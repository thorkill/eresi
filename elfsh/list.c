/*
** list.c for elfsh
** 
** Started on  Sat Jan 25 11:20:20 2003 mayhem
** Last update Wed Feb 19 10:12:48 2003 mayhem
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

  puts(" .::. Working files .::. ");
  index = 1;
  for (actual = world.list; actual != NULL; actual = actual->next, index++)
    {
      time = ctime(&actual->loadtime);
      nl = strchr(time, '\n');
      if (nl)
	*nl = 0x00;
      c = (world.current == actual ? '*' : ' ');
      printf(" [%03u] %s %c ID: %u %s \n", 
	     index, time, c, actual->id, actual->name);
    }
  if (!world.list)
    puts(" [*] No loaded file");
  puts("");
  vm_modlist();
  puts("");
  return (0);
}
