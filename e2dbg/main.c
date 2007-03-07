/*
** main.c for e2dbg
**
** The embedded ELF debugger
**
** Started on  Wed Feb 21 22:02:36 2001 mayhem
** Updated on  Wed Jan 03 17:51:04 2007 mayhem
*/
#include "e2dbg.h"

/* Execute the debuggee program */
int		vm_execute_debuggee(int ac, char **av)
{
  char		buf[BUFSIZ];
  char		*str;
  char          **args;
  int		index;
  char		*version;

#if defined(ELFSH32)
  version = "32";
#elif defined(ELFSH64)
  version = "64";
#else
  printf("Unknown debugger version : fatal error \n");
  exit(-1);
#endif

  args = alloca(ac * sizeof(char *));
  str = getenv("LD_PRELOAD");
  if (!str)
    snprintf(buf, sizeof(buf), "%s/libe2dbg%s.so", LIBPATH, version);
  else
    snprintf(buf, sizeof(buf), "%s/libe2dbg%s.so:%s", LIBPATH, version, str);
  setenv("LD_PRELOAD", buf, 1);
  for (index = 1; index < ac; index++)
    args[index - 1] = av[index];
  args[index - 1] = NULL;
  execve(args[0], args, environ);
  perror("exec");
  exit(-1);
}


/* The real main function */
int		vm_main(int ac, char **av)
{
  vm_setup(ac, av, REVM_STATE_DEBUGGER, REVM_SIDE_CLIENT);
  vm_print_banner();
  vm_config();
  if (!fork())
    vm_execute_debuggee(ac, av);
  vm_output(" [*] Type help for regular commands \n\n");
  return (vm_run(ac, av));
}


/* The main ELFsh routine */
int		main(int ac, char **av)
{
  return (vm_main(ac, av));
}



