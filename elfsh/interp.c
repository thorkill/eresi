/*
** interp.c for elfsh
** 
** Started on  Fri Nov  2 15:20:41 2001 mayhem
** Last update Tue Feb 18 15:44:21 2003 mayhem
*/
#include "elfsh.h"




/* Display the interpretor (.interp) section */
int		cmd_interp()
{
  char		*name;

  if ((name = elfsh_get_interp(world.current)) == NULL)
    RET(-1);
  printf("\n [SHT_INTERP] : %s \n\n", name);
  return (0);
}
