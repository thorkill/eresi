/*
** @file log.c
**
** Implement logging facilities 
**
** Started September 16 03:11:04 2005 mm
**
** $Id: log.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"

/**
 *  Enable logging 
 */
int			cmd_log()
{
  int			fd;
  char			buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Change logging state */
  if (world.curjob->curcmd->param[0])
    {

      /* Stop logging */
      if (!strcmp(world.curjob->curcmd->param[0], "stop"))
	revm_closelog();
      
      /* Log into a new file */
      else
	{
	  XOPEN(fd, world.curjob->curcmd->param[0], 
		O_WRONLY | O_CREAT | O_TRUNC, 0600, -1);
	  world.curjob->ws.state |= REVM_JOB_LOGGED;
	  world.curjob->ws.logfd = fd;
	  if (!world.state.revm_quiet)
	    {
	      snprintf(buf, BUFSIZ, " [*] Started logging session in %s \n\n", 
		       world.curjob->curcmd->param[0]);
	      revm_output(buf);
	    }
	}
    }

  /* List logging state */
  else
    {
      snprintf(buf, BUFSIZ, " [*] Session logging %s \n\n",
	       ((world.curjob->ws.state & REVM_JOB_LOGGED) ? 
		"enabled" : "disabled"));
      revm_output(buf);
    }
      
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Export in environment command 
 */
int		cmd_export()
{
  int		err;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  snprintf(buf, BUFSIZ, "%s=%s", world.curjob->curcmd->param[0], world.curjob->curcmd->param[1]);

  err = putenv(buf);
  
  if (!err && !world.state.revm_quiet)
    { 
      snprintf(buf, BUFSIZ, " [*] Exported %s to value %s \n\n",
	       world.curjob->curcmd->param[0], 
	       world.curjob->curcmd->param[1]);
      revm_output(buf);
    }
  if (!err)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to change environment", -1);
}
