/*
** log.c for elfsh
**
** Implement logging facilities 
**
** Started September 16 03:11:04 2005 mm
*/
#include "libui.h"

static void		logtofile(char *str)
{
  u_int			i, pos, len = 0;

  ELFSH_NOPROFILE_IN();

  /* Clean color parts */
  len = strlen(str);
  if (len > 0 && strchr(str, C_STARTCOLOR) != NULL)
    {
      char tmp[len+1];
      for (pos = 0, i = 0; i < len; i++)
	{
	  if (str[i] == C_STARTCOLOR)
	    {
	      while (i < len && str[i] != 'm')
		i++;
	    }
	  else
	    tmp[pos++] = str[i];
	}

      tmp[pos] = 0;

      str = tmp;

      /* reset length */
      len = strlen(str);
    }

  if (world.curjob->state & ELFSH_JOB_LOGGED)
    XWRITE(world.curjob->logfd, str, len, );

  ELFSH_NOPROFILE_OUT();
}


/* Log a line */
void			vm_log(char *str)
{
  int			check = 0;
  int			count = 0;

  ELFSH_NOPROFILE_IN();

  if (!str || !world.curjob ||
      !world.curjob->io.outcache.lines ||
      !world.curjob->io.outcache.cols)
    ELFSH_NOPROFILE_OUT();

  logtofile(str);

  count = vm_color_count(str);

  /* Allocate the screen buffer */
  if (world.curjob->screen.buf == NULL)
    {
      XALLOC(world.curjob->screen.buf,
	     world.curjob->io.outcache.lines *
	     world.curjob->io.outcache.cols + 1, );

      world.curjob->screen.x = world.curjob->io.outcache.cols;
      world.curjob->screen.y = world.curjob->io.outcache.lines;

      world.curjob->screen.head = world.curjob->screen.tail =
      world.curjob->screen.buf;

    }
  /* reallocate the screen buffer when term size changes */
  else if (world.curjob->screen.x != world.curjob->io.outcache.cols ||
	   world.curjob->screen.y != world.curjob->io.outcache.lines)
    {
      
      XFREE(world.curjob->screen.buf);
      XALLOC(world.curjob->screen.buf,
	     (world.curjob->io.outcache.lines+count) *
	     world.curjob->io.outcache.cols + 1, );

      world.curjob->screen.x = world.curjob->io.outcache.cols;
      world.curjob->screen.y = world.curjob->io.outcache.lines;

      world.curjob->screen.head = world.curjob->screen.tail =
	world.curjob->screen.buf;
    }

  /* We fill the buffer */
#define scrsize (world.curjob->io.outcache.cols * world.curjob->io.outcache.lines)
#define buf	world.curjob->screen.buf
#define tail	world.curjob->screen.tail
#define head	world.curjob->screen.head

  if (tail >= head)
    {
      // [head] ... [tail]
      if (tail + strlen(str) > buf + scrsize)
	{
	  snprintf(tail, scrsize - (tail - buf), "%s", str);
		check = strlen(str) - (scrsize - (tail - buf));
		if(check < 0)   ELFSH_NOPROFILE_OUT();

	  snprintf(buf, check, "%s", str);

	  tail = buf + strlen(str) - ((scrsize + buf) - tail);


	  if (tail > head)
	    head = tail + 1;

	}
      else
	{
	  sprintf(tail, "%s", str);
	  tail += strlen(str);
	}
    }
  else 
    // [tail] ... [head]
    {
      if (tail + strlen(str) > head)
	{
	  head = buf + ((tail - buf) + strlen(str))%scrsize;
	  sprintf(tail,"%s", str);
	  tail += strlen(str);

	}
      else
	{
	  if (tail + strlen(str) > buf + scrsize)
	    {
	      snprintf(tail, scrsize - (tail - buf), "%s", str);
				check = strlen(str) - (scrsize - (tail - buf));
				if(check < 0)   ELFSH_NOPROFILE_OUT();
	      snprintf(buf, check, "%s", str);
	      tail = buf + strlen(str) - ((scrsize + buf) - tail);

	      if (tail > head)
		head = tail + 1;
	    }
	  else
	    {
	      sprintf(tail, "%s", str);
	      tail += strlen(str);
	    }
	}
    }

#undef scrsize
#undef buf
#undef tail
#undef head


  ELFSH_NOPROFILE_OUT();
}


/* Stop logging */
int			vm_closelog()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XCLOSE(world.curjob->logfd, -1);

  if (!world.state.vm_quiet)
    vm_output(" [*] Saved logging session \n\n");
  world.curjob->state &= (~ELFSH_JOB_LOGGED);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


