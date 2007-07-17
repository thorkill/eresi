/*
** modlist.c for librevm in ERESI
**
** Started on  Wed Feb 19 04:42:47 2003 mayhem
**
** $Id: modlist.c,v 1.3 2007-07-17 18:11:24 may Exp $
**
*/
#include "revm.h"


/* List the loaded ERESI modules */
int             revm_modlist()
{
  revmmod_t     *actual;
  u_int         index;
  char          *time;
  char          *nl;
  char          logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output(" .::. ELFsh modules .::. \n");
  index = 1;
  for (actual = world.modlist; actual != NULL; actual = actual->next, index++)
    {
      time = ctime(&actual->loadtime);
      nl = strchr(time, '\n');
      if (nl)
        *nl = 0x00;
      snprintf(logbuf, BUFSIZ - 1, " [%03u] %s   ID: %u [%s] \n",
	       index, time, actual->id, actual->path);
      revm_output(logbuf);
    }
  if (world.modlist == NULL)
    revm_output(" [*] No loaded module\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
