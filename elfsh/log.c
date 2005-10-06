/*
** log.c for elfsh
**
** Implement logging facilities 
**
** Started September 16 03:11:04 2005 mm
*/
#include "elfsh.h"


/* Log a line */
void			vm_log(char *str)
{
  u_int			len = 0;
  int			ret = 0;
  u_int			delta;
  

  ELFSH_NOPROFILE_IN();

  if (!str || !world.curjob || 
      !world.curjob->io.outcache.lines || 
      !world.curjob->io.outcache.cols)
    return;

  len = strlen(str);
  if (world.curjob->state & ELFSH_JOB_LOGGED)
    {
      ret = write(world.curjob->logfd, str, len);
      if (len != ret)
	perror("write");
    }

  /* Allocate the screen buffer */
  if (world.curjob->screen.buf == NULL)
    {
      //printf("x : %d y : %d\n", world.curjob->io.outcache.lines, world.curjob->io.outcache.cols);

      XALLOC(world.curjob->screen.buf, 
	     world.curjob->io.outcache.lines * 
	     world.curjob->io.outcache.cols + 1, 
	     -1);

      if (!world.curjob->screen.buf)
	{
	  perror("malloc");
	  return;
	}
      world.curjob->screen.x = world.curjob->io.outcache.cols; 
      world.curjob->screen.y = world.curjob->io.outcache.lines;
      
      world.curjob->screen.head = world.curjob->screen.tail = world.curjob->screen.buf;

    }

  /* reallocate the screen buffer when term size changes */
  else if (world.curjob->screen.x != world.curjob->io.outcache.cols ||
	   world.curjob->screen.y != world.curjob->io.outcache.lines)
    {

      printf("resizing needed ...\n");
      return ;
      /*
	XREALLOC !!
      world.curjob->screen.buf = realloc(world.curjob->screen.buf,
					 world.curjob->io.outcache.cols *
					 world.curjob->io.outcache.lines);

      if (!world.curjob->screen.buf)
        {
          perror("realloc");
          return;
        }
      */
    }

  /*
  {
    char buf[256];
    snprintf(buf, 256, "\033[00;01;34m%#x\033[00m", world.curjob);
    vm_output_nolog(buf);
  }
  */

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
	  //printf("tail + strlen(str) > buf + scrsize\n");
	  // tail va revenir au debut du buffer (donc une partie concatene apres tail et une au debut)	  
	  snprintf(tail, scrsize - (tail - buf), "%s", str);
	  snprintf(buf, strlen(str) - scrsize - (tail - buf), "%s", str);
	  tail = buf + strlen(str) - ((scrsize + buf) - tail);

	  // peut depasser head ...
	  if (tail > head)
	    head = tail + 1;

	}
      else
	{
	  //printf("tail + strlen(str) <= buf + scrsize\n");
	  // tout a la fin et on avance tail 
	  
	  sprintf(tail, "%s", str);
	  tail += strlen(str);
	}
    }
  else 
    // [tail] ... [head]
    {
      if (tail + strlen(str) > head)
	{
	  //printf("tail + strlen(str) > head\n");
	  // on depasse head 
	  // donc on le bouge 
	  
	  // XXX cas ou tail fait le tour egalement
	  head = buf + ((tail - buf) + strlen(str))%scrsize;
	  sprintf(tail,"%s", str);
	  tail += strlen(str);
	  
	}
      else
	{
	  //printf("tail + strlen(str) <= head\n");
	  if (tail + strlen(str) > buf + scrsize)
	    {
	      // cas ou tail fait le tour egalement
	      snprintf(tail, scrsize - (tail - buf), "%s", str);
	      snprintf(buf, strlen(str) - scrsize - (tail - buf), "%s", str);
	      tail = buf + strlen(str) - ((scrsize + buf) - tail);

	      if (tail > head)
		head = tail + 1;
	    }  
	  else
	    {
	      // ok suffit de mettre str apres tail et de bouger tail
	      sprintf(tail, "%s", str);
	      tail += strlen(str);
	    }
	}
    }

#undef scrsize
#undef buf
#undef tail
#undef head


  /*
    if (world.curjob->screen.head - world.curjob->screen.tail + strlen(str) > scrsize)
    
    if (world.curjob->screen.cur + len <= scrsize)
    {
    memcpy(world.curjob->screen.buf + world.curjob->screen.cur, str, len);
    world.curjob->screen.cur += len;
    }
    else
    {
    delta = world.curjob->screen.cur + len - scrsize;
    memmove(world.curjob->screen.buf, world.curjob->screen.buf + delta, scrsize - delta);
    memcpy(world.curjob->screen.buf + scrsize - delta, str, ?? );
    
    // XXX: fixme
    
    world.curjob->screen.cur = scrsize;
    }
  */

  //ELFSH_NOPROFILE_ROUT();
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



/* Enable logging */
int			cmd_log()
{
  int			fd;
  char			buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Change logging state */
  if (world.curjob->curcmd->param[0])
    {

      /* Stop logging */
      if (!strcmp(world.curjob->curcmd->param[0], "stop"))
	vm_closelog();
      
      /* Log into a new file */
      else
	{
	  XOPEN(fd, world.curjob->curcmd->param[0], 
		O_WRONLY | O_CREAT, 0600, -1);
	  world.curjob->state |= ELFSH_JOB_LOGGED;
	  world.curjob->logfd = fd;
	  if (!world.state.vm_quiet)
	    {
	      snprintf(buf, BUFSIZ, " [*] Started logging session in %s \n\n", 
		       world.curjob->curcmd->param[0]);
	      vm_output(buf);
	    }
	}
    }

  /* List logging state */
  else
    {
      snprintf(buf, BUFSIZ, " [*] Session logging %s \n\n",
	       ((world.curjob->state & ELFSH_JOB_LOGGED) ? 
		"enabled" : "disabled"));
      vm_output(buf);
    }
      
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Export in environment command */
int		cmd_export()
{
  int		err;
  char		buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  err = setenv(world.curjob->curcmd->param[0], 
	       world.curjob->curcmd->param[1], 1);
  
  if (!err && !world.state.vm_quiet)
    { 
      snprintf(buf, BUFSIZ, " [*] Exported %s to value %s \n\n",
	       world.curjob->curcmd->param[0], 
	       world.curjob->curcmd->param[1]);
      vm_output(buf);
    }
  if (!err)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  else
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to change environment", -1);
}

