/*
** main.c for e2dbg
**
** The embedded ELF debugger
**
** Started on  Wed Feb 21 22:02:36 2001 mayhem
** Updated on  Wed Jan 03 17:51:04 2007 mayhem
*/
#include "e2dbg.h"

/* The real main function */
int		vm_main(int ac, char **av)
{
  vm_setup(ac, av, REVM_STATE_DEBUGGER, REVM_SIDE_CLIENT);
  vm_print_banner(av[1]);
  vm_config();
  vm_output(" [*] Type help for regular commands \n\n");
  return (vm_run(ac, av));
}


/* The main ELFsh routine */
int		main(int ac, char **av)
{
  return (vm_main(ac, av));
}



