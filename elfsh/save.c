/*
** save.c for elfsh
** 
** Started on  Sat Jan 25 11:22:35 2003 mayhem
** Last update Wed Feb 26 15:35:55 2003 mayhem
*/
#include "elfsh.h"


/* Save objects which name is given as parameter */
int		cmd_save()
{
  elfsh_save_obj(world.current, world.args.param[0]);
  printf("\n [*] Object %s save successfully \n\n", 
	 world.args.param[0]);
  return (0);
}
