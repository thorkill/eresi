/**
 * @defgroup e2dbg e2dbg
 */
/*
 * @file main.c
 * @ingroup e2dbg
 * The embedded ELF debugger
 *
 * Started on  Wed Feb 21 22:02:36 2001 jfv
 * Updated on  Wed Jan 03 17:51:04 2007 jfv
 *
 * $Id: main.c,v 1.20 2008-02-16 12:30:23 thor Exp $
 *
 */
#include "e2dbg.h"


char		*version;


/**
 * Setup LD_PRELOAD for dynamic binaries debugging 
 */
void		revm_debugger_preload()
{
#if defined(sun)
  char		envbuf[BUFSIZ / 2];
#endif
  int		ret;
  char		buf[BUFSIZ / 2];
  char		*str;

  /* Prepare the new LD_PRELOAD -- 
     use --enable-testing to test e2dbg without install */
  str = getenv("LD_PRELOAD");
  if (!str || !*str)
    snprintf(buf, sizeof(buf), "%s/libe2dbg%s.so", E2DBG_PATH, version);
  else
    snprintf(buf, sizeof(buf), "%s/libe2dbg%s.so:%s", E2DBG_PATH, version, str);

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



/**
 * Inject the .o debugger file into the static binary 
 * @param file
 * @return
 */
char*		revm_debugger_inject(elfshobj_t *file)
{
  char		*buf;
  elfshobj_t	*reloc;
  int		buflen;
  char		*ptr;
  char		*ptr2;
  
  /* Map host file */
  ptr = ptr2 = NULL;
  buflen = strlen(E2DBG_PATH) + strlen(file->name) + 20;
  buf = calloc(buflen, 1);
  snprintf(buf, buflen, "%s/e2dbg-rel%s.o", E2DBG_PATH, version);
  reloc = elfsh_map_obj(buf);
  if (!reloc)
    {
      fprintf(stderr, 
	      " [E] Static target and unable to load %s/e2dbg-rel%s.o \n",
	      E2DBG_PATH, version);
      return (NULL);
    }
  fprintf(stderr, " [*] Now injecting debugger in target binary .. please wait .. \n");

  /* Inject debugger in target file */
  //profiler_enable_err();
  //profiler_enable_out();

  //sleep(5);

  if (elfsh_inject_etrel(file, reloc) < 0)
    {
      fprintf(stderr, 
	      " [E] Target binary is static and unable to inject e2dbg-rel%s.o \n",
	      version);
      return (NULL);
    }

  //profiler_disable_err();
  //profiler_disable_out();

  snprintf(buf, buflen, "%s.dbg", file->name);
  fprintf(stderr, " [*] Now saving target binary : %s \n", buf);

  /* Save object to be debugged */
  if (elfsh_save_obj(file, buf) < 0)
    {
      ptr = file->name;
      while ((ptr = strchr(ptr, '/')))
	ptr2 = ++ptr;
      if (!ptr2)
	ptr2 = file->name;
      snprintf(buf, buflen, "/tmp/%s.dbg", ptr2);
      fprintf(stderr, " [*] Now saving target binary : %s \n", buf);
      if (elfsh_save_obj(file, buf) < 0)
	return (NULL);
    }

  return buf;
}



/**
 * Execute the debuggee program 
 * @param ac Number of arguments.
 * @param av Array of arguments.
 * @return
 */
int		revm_execute_debuggee(int ac, char **av)
{
  char          **args;
  int		index;
  elfshobj_t	*file;

#if defined(ERESI32)
  version = "32";
#elif defined(ERESI64)
  version = "64";
#else
  printf("Unknown debugger version : fatal error \n");
  exit(-1);
#endif

  /* Map the debugger in the debuggee */
  file = elfsh_map_obj(av[1]);
  if (!file || !elfsh_static_file(file))
    revm_debugger_preload();
  else
    {
      av[1] = revm_debugger_inject(file);
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


/**
 * The real main function 
 * @param ac
 * @param av
 * @return
 */
int		e2dbg_main(int ac, char **av)
{
  pid_t		pid;
  int		status;

  revm_setup(ac, av, REVM_STATE_DEBUGGER, REVM_SIDE_CLIENT);
  revm_config(E2DBG_CONFIG);
  pid = fork();
  if (!pid)
    revm_execute_debuggee(ac, av);
  else
    {

      usleep(50000);
      
      // debugging purpose
      //sleep(20);

      if (waitpid(pid, &status, WNOHANG) != 0)
	{
	  revm_output("\n [E] Target binary not found\n");
	  revm_output("\n Syntax : ");
	  revm_output(av[0]);
	  revm_output(" target_binary \n\n");
	  exit(-1);
	}
    }

  revm_output(" [*] Type help for regular commands \n\n");
  revm_run(ac, av);
  return (0);
}


/**
 * The starting E2dbg routine 
 * @param ac
 * @param av
 * @return
 */
int		main(int ac, char **av)
{
  //fprintf(stderr, "e2dbg client main -----------> \n");
  return (e2dbg_main(ac, av));
}



