/*
** save.c for elfsh
** 
** Started on  Sat Jan 25 11:22:35 2003 mayhem
** Last update Wed Mar 10 12:32:35 2004 mayhem
*/
#include "elfsh.h"


/* Save objects which name is given as parameter */
int		cmd_save()
{
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_save_obj(world.curjob->current, 
		     world.curjob->curcmd->param[0]) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to save object", -1);

  snprintf(logbuf, BUFSIZ - 1, "\n [*] Object %s saved successfully \n\n", 
	   world.curjob->curcmd->param[0]);
  vm_output(logbuf);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
