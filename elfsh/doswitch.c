/*
** doswitch.c for elfsh
** 
** Started on  Sat Jan 25 11:20:49 2003 mayhem
** Last update Wed Feb 26 15:56:41 2003 mayhem
*/
#include "elfsh.h"


/* Change the current object */
int		cmd_doswitch()
{
  elfshobj_t	*actual;
  int		nbr;

  if (world.args.param[0] == NULL)
    return (-1);
  nbr = atoi(world.args.param[0]);
  actual = (nbr ? vm_getfile(nbr) : hash_get(&file_hash, world.args.param[0]));
  if (actual == NULL)
    {
      printf("\n [!] Cant switch on file object %s (use 'list') \n\n", 
	     world.args.param[0]);
      return (-1);
    }
  
  printf("\n [*] Switched on object %u (%s) \n\n", actual->id, actual->name);
  world.current = actual;
  return (0);
}
