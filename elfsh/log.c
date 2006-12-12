/*
** log.c for elfsh
**
** Implement logging facilities 
**
** Started September 16 03:11:04 2005 mm
*/
#include "elfsh.h"

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
		O_WRONLY | O_CREAT | O_TRUNC, 0600, -1);
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
  
  snprintf(buf, BUFSIZ, "%s=%s", world.curjob->curcmd->param[0], world.curjob->curcmd->param[1]);

  err = putenv(buf);
  
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
