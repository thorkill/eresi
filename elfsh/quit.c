/*
** quit.c for elfsh
** 
** Started on  Sat Jan 25 11:19:53 2003 mayhem
** Last update Tue Feb 18 05:15:51 2003 mayhem
*/
#include "elfsh.h"


/* Unload all the loaded object and exit the shell */
int		cmd_quit()
{
  elfshobj_t	*cur;
  elfshobj_t	*tmp;
  int		index;

  puts("");
  for (index = 1, cur = world.list; cur; cur = tmp, index++)
    {
      printf(" [*] Unloading object %u (%s) %c \n", 
	     index, cur->name, 
	     (world.current == cur ? '*' : ' '));
      tmp = cur->next;
      elfsh_unload_obj(cur);
    }
  printf("\n\t Good bye ! .::. The ELF shell %s \n\n", ELFSH_VERSION);
  exit(0);
}
