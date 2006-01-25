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
  char		optbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  index = 1;

  /* Private descriptors */
  if (world.curjob->list)
    vm_output(" .::. Static Working files .::. \n");
  for (actual = world.curjob->list; actual != NULL; actual = actual->next, index++)
    {
      time = ctime(&actual->loadtime);
      nl = strchr(time, '\n');
      if (nl)
	*nl = 0x00;
      c = (world.curjob->current == actual ? '*' : ' ');
      c2 = ((actual->linkmap || actual->rhdr.base) ? 'M' : ' ');
      if (elfsh_is_debug_mode())
	snprintf(optbuf, BUFSIZ, "(" XFMT ")", actual->rhdr.base);
      else
	snprintf(optbuf, BUFSIZ, "%s", "");

     snprintf(logbuf, BUFSIZ - 1, " %s %c%c %s ID: %2u %s %s \n", 
	      time, c, c2, optbuf, actual->id, 
	      elfsh_get_objtype(actual->hdr) == ET_REL  ? "ET_REL " : 
	      elfsh_get_objtype(actual->hdr) == ET_DYN  ? "ET_DYN " : 
	      elfsh_get_objtype(actual->hdr) == ET_EXEC ? "ET_EXEC" : 
	      elfsh_get_objtype(actual->hdr) == ET_CORE ? "ET_CORE" : 
	      "UNKNOWN", actual->name);
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
      if (elfsh_is_debug_mode())
	snprintf(optbuf, BUFSIZ, "(" XFMT ")", actual->rhdr.base);
      else
	snprintf(optbuf, BUFSIZ, "%s", "");

     snprintf(logbuf, BUFSIZ - 1, " [%02u] %s %c%c %s ID: %02u %s \n", 
	     index, time, c, c2, optbuf, actual->id, actual->name);
     vm_output(logbuf);
    }

  
  if (!world.curjob->list && !world.shared)
    vm_output(" [*] No loaded file\n");
  vm_output("\n");
  vm_modlist();
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
