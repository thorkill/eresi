/*
** interp.c for elfsh
** 
** Started on  Fri Nov  2 15:20:41 2001 mayhem
** Last update Wed Mar 10 12:21:34 2004 mayhem
*/
#include "elfsh.h"




/* Display the interpretor (.interp) section */
int		cmd_interp()
{
  char		*name;
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if ((name = elfsh_get_interp(world.curjob->current)) == NULL)
    RET(-1);
  snprintf(logbuf, BUFSIZ - 1, "\n [SHT_INTERP] : %s \n\n", name);
  vm_output(logbuf);
  return (0);
}
