/*
** fcthijack.c for elfsh
** 
** Started on  Thu Jun 19 17:02:55 2003 mayhem
** Last update Sun Jun 22 01:48:18 2003 mayhem
*/
#include "elfsh.h"


/* Redirect a function on a OS independant manner */
int		cmd_hijack()
{
  Elf32_Sym	*dst;
  u_long	addr;
  int		err;
  char		*rev;

  /* Resolve destination parameter */
  dst = elfsh_get_metasym_by_name(world.current, world.args.param[1]);
  if (dst == NULL)
    {
      err = sscanf(world.args.param[1], "0x%08X", (unsigned int *) &addr);
      if (err != 1)
	return (-1);
      rev = vm_reverse(world.current, addr);
    }
  else
    {
      addr = dst->st_value;
      rev = NULL;
    }

  /* Hijack function */
  err = elfsh_hijack_function_by_name(world.current, 
				      ELFSH_HIJACK_TYPE_PLT |  
				      ELFSH_HIJACK_OS_ANY   |  
				      ELFSH_HIJACK_CPU_ANY, 
				      world.args.param[0], 
				      addr);

  /* Last checks */
  if (err < 0)
    return (-1);
  if (!world.state.vm_quiet)
    printf(" [*] Function %s redirected to addr 0x%08X <%s> \n\n", 
	   world.args.param[0], (unsigned int) addr, 
	   (rev == NULL ? world.args.param[1] : rev));
  if (rev != NULL)
    free(rev);
  
  return (0);
}
