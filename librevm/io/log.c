/**
 * @file log.c
 * @ingroup io
 * @brief Implement logging facilities.
 *
 * Started September 16 03:11:04 2005 jfv
 *
 * $Id: log.c,v 1.10 2007-11-29 14:01:56 may Exp $
 *
 */
#include "revm.h"


/* Strip a group of char */
static void		__strip_group_char(char *str, char s, char e)
{
  unsigned int			len, diff, pos;
  char			*search, *search_end;

  NOPROFILER_IN();
  len = strlen(str) + 1;
  for (search = str; (search = strchr(search, s)) != NULL;)
    {
      search_end = strchr(search+1, e);
      /* Error !! */
      if (!search_end)
	NOPROFILER_OUT();
      diff = search_end - search + 1;
      pos = search - str;
      /* Realign others */
      memmove(search, search_end+1, len - (pos + diff));
      len -= diff;
    }
  NOPROFILER_OUT();
}


/**
 * Log to file
 */
static void		logtofile(char *str)
{
  revmexpr_t		*stripexpr;
  revmobj_t		*stripvar;
  char			*tmp;
  unsigned int			len;

  NOPROFILER_IN();
  len = strlen(str);
  tmp = alloca(len + 1);

  /* Do we want to log ? */
  if (!(world.curjob->ws.state & REVM_JOB_LOGGED) || len <= 0)
    NOPROFILER_OUT();

  /* We made only local modifications */
  strcpy(tmp, str);
  stripexpr = revm_expr_get(REVM_VAR_STRIPLOG);
  if (stripexpr)
    stripvar = stripexpr->value;
  
  /* We strip the color part depending of $SLOG variable value */
  if (stripvar && stripvar->immed_val.word)
    __strip_group_char(tmp, C_STARTCOLOR, 'm');

  /* Strip RL_PROMPT_START_IGNORE & RL_PROMPT_END_IGNORE */
  revm_strip_char(tmp, RL_PROMPT_START_IGNORE);
  revm_strip_char(tmp, RL_PROMPT_END_IGNORE);

  len = strlen(tmp);
  XWRITE(world.curjob->ws.logfd, tmp, len, );
  NOPROFILER_OUT();
}


/**
 * Log a line 
 */
void			revm_log(char *str)
{
  int			check = 0, size = 0;
  char			*tmp = NULL;
  char 			*t_tail, *t_head, *t_end;
  char			buf[BUFSIZ * 4];

  NOPROFILER_IN();

  /* Nothing possible */
  if (!str || !*str || !world.curjob)
    NOPROFILER_OUT();

  logtofile(str);

  /* We could log before return */
  if(!world.curjob->ws.io.outcache.lines ||
     !world.curjob->ws.io.outcache.cols)
    NOPROFILER_OUT();

  /* Allocate the screen buffer */
  if (world.curjob->ws.screen.buf == NULL)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__,
	     world.curjob->ws.screen.buf,
	     world.curjob->ws.io.outcache.lines *
	     world.curjob->ws.io.outcache.cols + 1, );

      world.curjob->ws.screen.x = world.curjob->ws.io.outcache.cols;
      world.curjob->ws.screen.y = world.curjob->ws.io.outcache.lines;
      world.curjob->ws.screen.head = world.curjob->ws.screen.tail =
      world.curjob->ws.screen.buf;
    }

  /* reallocate the screen buffer when term size changes */
  else if (world.curjob->ws.screen.x != world.curjob->ws.io.outcache.cols ||
	   world.curjob->ws.screen.y != world.curjob->ws.io.outcache.lines)
    {
      tmp = world.curjob->ws.screen.buf;
      XALLOC(__FILE__, __FUNCTION__, __LINE__,world.curjob->ws.screen.buf,
	     world.curjob->ws.io.outcache.lines *
	     world.curjob->ws.io.outcache.cols + 1, );

      /* Save ex pointers */
      t_head = world.curjob->ws.screen.head;
      t_tail = world.curjob->ws.screen.tail;
      t_end = tmp + (world.curjob->ws.screen.x * world.curjob->ws.screen.y);

      /* Update screen */
      world.curjob->ws.screen.x = world.curjob->ws.io.outcache.cols;
      world.curjob->ws.screen.y = world.curjob->ws.io.outcache.lines;

      /* Reset pointers */
      world.curjob->ws.screen.head = world.curjob->ws.screen.tail =
	world.curjob->ws.screen.buf;

      /* Refill the screen using revm_log() */
      if (t_head < t_tail)
	revm_log(t_head);
      else
	{
	  snprintf(buf, t_end - t_head, "%s", t_head);
	  revm_log(buf);
	  snprintf(buf, BUFSIZ * 4 - 1, "%s", t_tail);
	  revm_log(buf);
	}
      
      /* Free at the end */
      XFREE(__FILE__, __FUNCTION__, __LINE__,tmp);
    }

#define scrsize (world.curjob->ws.io.outcache.cols * world.curjob->ws.io.outcache.lines)
#define buf	world.curjob->ws.screen.buf
#define tail	world.curjob->ws.screen.tail
#define head	world.curjob->ws.screen.head

  //printf("BUF = %08x HEAD = %08x TAIL = %08x\n", buf, head, tail);

  /* We could have to loop many time before full fill all the buffer */
  for (size = strlen(str), check = 0; size > 0; size -= check, str += check)
    {
      check = 0;
      /* Tail is after head or we can fill tail without change (or not a lot) the head */
      if (tail >= head || (tail + strlen(str) <= head && head < buf + scrsize))
	{
	  /* There is not enough space ! */
	  if (tail + strlen(str) > buf + scrsize)
	    {
	      /* We can copy a part of the buffer */
	      check = scrsize - (tail - buf);
	      snprintf(tail, check, "%s", str);

	      /* Tail is know at the beginning */
	      tail = buf;

	      /* If head is on the beginning */
	      if (tail >= head)
		head = tail + 1;
	    }
	  else
	    {
	      /* We can copy the whole buffer */
	      snprintf(tail, scrsize, "%s", str);
	      tail += strlen(str);
	      break;
	    }
	}
      else
	{
	  /* What size we can use for the moment ? */
	  check = buf + scrsize - tail;

	  /* May be we need less */
	  if (check > strlen(str))
	    check = strlen(str);

	  /* Adjust head pointer */
	  head = buf + ((tail - buf) + check)%scrsize;
	  tail += check;
	}
    }

#undef scrsize
#undef buf
#undef tail
#undef head


  NOPROFILER_OUT();
}


/**
 * Stop logging 
 */
int			revm_closelog()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XCLOSE(world.curjob->ws.logfd, -1);

  if (!world.state.revm_quiet)
    revm_output(" [*] Saved logging session \n\n");
  world.curjob->ws.state &= (~REVM_JOB_LOGGED);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


