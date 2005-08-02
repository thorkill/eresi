/*
** list.c for elfsh
** 
** Started on  Sat Jan 25 11:20:20 2003 mayhem
*/
#include "elfsh.h"


/* List all the loaded objects */
int		cmd_dolist()
{
  elfshobj_t	*actual;
  int		index;
  char		*time;
  char		*nl;
  char		c;
  char		c2;
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  index = 1;

  /* Private descriptors */
  if (world.curjob->list || !world.shared)
    vm_output(" .::. Working files .::. \n");
  for (actual = world.curjob->list; actual != NULL; actual = actual->next, index++)
    {
      time = ctime(&actual->loadtime);
      nl = strchr(time, '\n');
      if (nl)
	*nl = 0x00;
      c = (world.curjob->current == actual ? '*' : ' ');
      c2 = ((actual->linkmap||actual->base) ? 'D' : ' ');
     snprintf(logbuf, BUFSIZ - 1, " [%03u] %s %c%c ID: %u %s \n", 
	     index, time, c, c2, actual->id, actual->name);
     vm_output(logbuf);
    }

  /* Shared descriptors */
  if (world.shared)
    vm_output("\n .::. Shared Working files .::. \n");
  for (actual = world.shared; actual != NULL; actual = actual->next, index++)
    {
      time = ctime(&actual->loadtime);
      nl = strchr(time, '\n');
      if (nl)
	*nl = 0x00;
      c = (world.curjob->current == actual ? '*' : ' ');
      c2 = (actual->linkmap ? 'L' : ' ');
     snprintf(logbuf, BUFSIZ - 1, " [%03u] %s %c%c ID: %u %s \n", 
	     index, time, c, c2, actual->id, actual->name);
     vm_output(logbuf);
    }

  if (!world.curjob->list && !world.shared)
    vm_output(" [*] No loaded file\n");
  vm_output("\n");
  vm_modlist();
  vm_output("\n");
  return (0);
}
