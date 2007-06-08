/*
** main.c for e2dbg
**
** The embedded ELF debugger
**
** Started on  Wed Feb 21 22:02:36 2001 mayhem
** Updated on  Wed Jan 03 17:51:04 2007 mayhem
**
** $Id: main.c,v 1.14 2007-06-08 00:27:33 may Exp $
**
*/
#include "e2dbg.h"


char		*version;


/* Setup LD_PRELOAD for dynamic binaries debugging */
void		vm_debugger_preload()
{
#if defined(sun)
  char		envbuf[BUFSIZ / 2];
#endif
  int		ret;
  char		buf[BUFSIZ / 2];
  char		*str;

  /* Prepare the new LD_PRELOAD */
  str = getenv("LD_PRELOAD");
  if (!str || !*str)
    snprintf(buf, sizeof(buf), "%s/libe2dbg%s.so", ELFSH_DBGPATH, version);
  else
    snprintf(buf, sizeof(buf), "%s/libe2dbg%s.so:%s", ELFSH_DBGPATH, version, str);

#if !defined(sun)
  ret = setenv("LD_PRELOAD", buf, 1);
#else
  snprintf(envbuf, sizeof(envbuf), "LD_PRELOAD=%s", buf);
  ret = putenv(envbuf);
#endif
  
  if (ret)
    fprintf(stderr, "Failed to preload libe2dbg\n");
  else
    fprintf(stderr, " [*] Preloading %s \n", buf);
}



/* Inject the .o debugger file into the static binary */
char*		vm_debugger_inject(elfshobj_t *file)
{
  char		*buf;
  elfshobj_t	*reloc;
  int		buflen;

  buflen = strlen(ELFSH_DBGPATH) + strlen(file->name) + 20;
  buf = calloc(buflen, 1);
  snprintf(buf, buflen, "%s/e2dbg-rel%s.o", ELFSH_DBGPATH, version);
  reloc = elfsh_map_obj(buf);

  if (!reloc)
    {
      fprintf(stderr, 
	      " [E] Static target and unable to load %s/e2dbg-rel%s.o \n",
	      ELFSH_DBGPATH, version);
      return (NULL);
    }
  fprintf(stderr, " [*] Now injecting debugger in target binary .. please wait .. \n");
  if (elfsh_inject_etrel(file, reloc) < 0)
    {
      fprintf(stderr, 
	      " [E] Target binary is static and unable to inject e2dbg-rel%s.o \n",
	      version);
      return (NULL);
    }
  fprintf(stderr, " [*] Now saving target binary .. \n");
  snprintf(buf, buflen, "%s.debuggee", file->name);
  if (elfsh_save_obj(file, buf) < 0)
    {
      snprintf(buf, buflen, "/tmp/%s.debuggee", file->name);
      if (elfsh_save_obj(file, buf) < 0)
	return (NULL);
    }
  return (buf);
}



/* Execute the debuggee program */
int		vm_execute_debuggee(int ac, char **av)
{
  char          **args;
  int		index;
  elfshobj_t	*file;

#if defined(ELFSH32)
  version = "32";
#elif defined(ELFSH64)
  version = "64";
#else
  printf("Unknown debugger version : fatal error \n");
  exit(-1);
#endif

  /* Map the debugger in the debuggee */
  file = elfsh_map_obj(av[1]);
  if (!file || !elfsh_static_file(file))
    vm_debugger_preload();
  else
    {
      av[1] = vm_debugger_inject(file);
      if (av[1] == NULL)
	exit(-1);
    }

  /* Execute the debuggee program */
  args = alloca(ac * sizeof(char *));
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
      
      // debugging purpose
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



