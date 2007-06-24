/*
** main.c for evarista
**
** The entry point of the static analyzer
**
** Started on  Fri Jun 22 14:42:36 2007 mayhem
** $Id: main.c,v 1.1 2007-06-24 14:38:50 may Exp $
*/
#include "evarista.h"

/* The real main function */
int		eva_main(int ac, char **av)
{
  vm_setup(ac, av, 0, 0);
  if (world.state.vm_mode != REVM_STATE_CMDLINE)
    vm_print_banner(av[1]);
  vm_config();
  setup_local_cmdhash();
  vm_output(" [*] Type help for regular commands \n\n");
  return (vm_run(ac, av));
}


/* The main ELFsh routine */
int		main(int ac, char **av)
{
  return (eva_main(ac, av));
}



