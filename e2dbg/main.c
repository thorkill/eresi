/*
** main.c for e2dbg
**
** The embedded ELF debugger
**
** Started on  Wed Feb 21 22:02:36 2001 mayhem
** Updated on  Wed Jan 03 17:51:04 2007 mayhem
**
** $Id: main.c,v 1.7 2007-04-02 17:59:22 may Exp $
**
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
    snprintf(buf, sizeof(buf), "%s/libe2dbg%s.so", ELFSH_LIBPATH, version);
  else
    snprintf(buf, sizeof(buf), "%s/libe2dbg%s.so:%s", ELFSH_LIBPATH, version, str);
  setenv("LD_PRELOAD", buf, 1);
  for (index = 1; index < ac; index++)
    args[index - 1] = av[index];
  args[index - 1] = NULL;
  execve(args[0], args, environ);
  exit(-1);
}


/* The real main function */
int		e2dbg_main(int ac, char **av)
{
  pid_t		pid;
  int		status;

  vm_setup(ac, av, REVM_STATE_DEBUGGER, REVM_SIDE_CLIENT);
  vm_config();
  pid = fork();
  if (!pid)
    vm_execute_debuggee(ac, av);
  else
    {

      usleep(50000);
      //sleep(20);

      if (waitpid(pid, &status, WNOHANG) != 0)
	{
	  vm_output("\n [E] Target binary not found\n");
	  vm_output("\n Syntax : ");
	  vm_output(av[0]);
	  vm_output(" target_binary \n\n");
	  exit(-1);
	}
    }

  vm_output(" [*] Type help for regular commands \n\n");
  return (vm_run(ac, av));
}


/* The main ELFsh routine */
int		main(int ac, char **av)
{
  //fprintf(stderr, "e2dbg client main -----------> \n");
  return (e2dbg_main(ac, av));
}



