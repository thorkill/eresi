/**
 * @file entry.c
 *
 * The debugger file for OS dependant entry points
 * 
 * Started on  Tue Jul 11 20:37:33 2003 jfv
 *
 *
 * $Id$: entry.c,v 1.1 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libe2dbg.h"




/**
 * Our fake main function 
 * @param argc
 * @param argv
 * @param aux
 * @return
 */
int			e2dbg_fake_main(int argc, char **argv, char **aux)
{
  e2dbgparams_t		params;
  char			*args[3];
  char			*pn;
  int			ret;

#if __DEBUG_E2DBG__
  char			logbuf[BUFSIZ];
  unsigned int		len;
  int			idx;
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__)
  pn = __progname;
#elif defined(sun)
  pn = getexecname();
#else
  pn = __progname_full;
#endif

#if __DEBUG_E2DBG__
  write(1, "Calling e2dbg_fake_main ! \n", 27);
  for (idx = 0; argv[idx]; idx++)
    {
      write(1, "argv = ", 7);
      write(1, argv[idx], strlen(argv[idx]));
      write(1, "\n", 1);
    }
  write(1, "__progname_full = ", 18);
  write(1, pn, strlen(pn));
  write(1, "\n", 1);
#endif

  /* Create the debugger thread */
  args[0] = E2DBG_ARGV0;
  args[1] = pn;
  //args[1] = argv[0];
  args[2] = NULL;
  params.ac = 2;
  params.av = args;

  e2dbg_presence_set();

  /* Initialize a "fake" thread if we are debugging a monothread program */
  if (e2dbgworld.curthread == NULL)
    e2dbg_curthread_init();
  
  /* Make sure libstderesi handlers are filled */
  eresi_stdconstructor();

  /* Call the debugger */
  e2dbg_entry(&params);

#if __DEBUG_E2DBG__
  write(1, "[(e2dbg_fake_main] Calling ON_EXIT \n", 46);
#endif

  /* Wait for debuggee exit */
  on_exit((void *) wait4exit, NULL);
  SETSIG;

#if __DEBUG_E2DBG__
  len = snprintf(logbuf, BUFSIZ, 
		 "[e2dbg_fake_main] Calling main %08X curthr %08X id %u\n", 
		 (eresi_Addr) e2dbgworld.real_main, 
		 (eresi_Addr) e2dbgworld.curthread, 
		 (unsigned int) getpid());
  write(1, logbuf, len);
#endif

  /* Call the original main */
  e2dbg_presence_reset();
  ret = (*e2dbgworld.real_main)(argc, argv, aux);
  
#if __DEBUG_E2DBG__
  len = snprintf(logbuf, BUFSIZ, 
		 "[e2dbg_fake_main] Main returned %u\n", ret);
  write(1, logbuf, len);
#endif

  /* Force PIE binaries to exit */
  if (world.curjob->curfile->hdr->e_type == ET_DYN)
    wait4exit(NULL);

  return (ret);
}




#if defined(linux)


/**
 * Entry point for Linux 
 * @param main
 * @param argc
 * @param ubp_av
 * @param init
 * @param fini
 * @param rtld_fini
 * @param stack_end
 * @return
 */
int	__libc_start_main(int (*main) (int, char **, char **aux),
			  int argc, char **ubp_av,
			  // FIXME on PPC
			  //ElfW(auxv_t) *__unbounded auxvec, 
			  void (*init) (void),
			  void (*fini) (void),
			  void (*rtld_fini) (void), 
			  void *__unbounded stack_end)
{
  eresi_Addr		orig;
  int			(*libcstartmain)();
  int			ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  e2dbg_presence_set();

#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] there\n", 33);
#endif

  /* Find the real symbol in libc */
  orig = (eresi_Addr) e2dbg_dlsym("__libc_start_main");
  if (!orig)
    {
      write(1, "Error : Orig __libc_start_main not found\n", 41);
      return (-1);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Orig __libc_start_main not found", (-1));
    }

  /* Save the libcstartmain for future references */
  libcstartmain = (void *) orig;

#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] there 2\n", 35);
#endif

  /* Load the debugger in a new thread */
  e2dbgworld.preloaded = 1;
  //CLRSIG;

  /* Initalize mutexes */
  e2dbg_mutex_init(&e2dbgworld.dbgbp);

#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] there 3 \n", 35);
#endif

  e2dbgworld.real_main = main;
  ret = libcstartmain(e2dbg_fake_main, argc, ubp_av, init, 
		      fini, rtld_fini, stack_end);

  /* Never reached */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(sun)


/**
 * Find the number of arguments by inspecting the environment on the stack 
 * @param args
 * @return
 */

int				e2dbg_get_args(char **args)
{
  char				*str;
  int				argc;
  int				index;

  str = ((char *) environ[0] - 2);
  argc = 0;

  /* First find the number of arguments */
 again:
  while (*str)
    str--;
  str++;
  argc++;
  if (access(str, F_OK))
    {
      str -= 2;
      goto again;
    }

  /* Then fill the argv array */
  args[0] = E2DBG_ARGV0;
  for (index = 1; index < argc + 1 && index < 42; index++)
    {
      args[index] = str;
      while (*str)
	str++;
      str++;
    }
  args[index] = NULL;
  return (argc);
}



/**
 * Entry point on FreeBSD 
 * @param fini
 * @return
 */
int				atexit(void (*fini)(void))
{
  eresi_Addr			orig;
  int				(*libc_atexit)();
  char				*args[42];
  static short int		bFusible = 0;
  e2dbgparams_t			params;
  int				argc;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  e2dbg_presence_set();
  
#if __DEBUG_E2DBG__
  printf("[(e2dbg)atexit] there\n");
#endif

  /* Find the real symbol */
  orig = (eresi_Addr) e2dbg_dlsym("atexit");
  if (!orig)
    {
      write(1, "Error : Orig atexit not found\n", 30);
      return (-1);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Orig atexit not found", (-1));
    }
  libc_atexit = (void *) orig;
  
#if __DEBUG_E2DBG__
  printf("[(e2dbg)atexit 2 : envp = %08X \n", (eresi_Addr) environ);
#endif

  /* Initialize a "fake" thread if we are debugging a monothread program */
  if (e2dbgworld.curthread == NULL)
    e2dbg_curthread_init();
  
#if __DEBUG_E2DBG__
  printf("[(e2dbg) atexit 3 \n");
#endif

  /* Load the debugger only one time */
  if (bFusible == 0)
    {
      bFusible = 1;

      /* Get argv / argc */
      argc = e2dbg_get_args(args);
      params.ac = argc;
      params.av = args;
      
      /* Load the debugger in a new thread */
      e2dbgworld.preloaded = 1;
      
      /* Initalize mutexes */
      e2dbg_mutex_init(&e2dbgworld.dbgbp);      
      e2dbg_entry(&params); 
    }
  
#if __DEBUG_E2DBG__
  printf("[e2dbg-fbsd: atexit 3]\n");
#endif

  /* Recall the original function */
  e2dbg_presence_reset();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     libc_atexit(fini));
}

/**
 * Entry point for Solaris 
 * @param argc
 * @param ubp_av
 * @return
 */
/*
  void			__fpstart(int argc, char **ubp_av)
  {
  eresi_Addr		orig;
  int			(*realfpstart)();
  char			*argv[3];
  e2dbgparams_t		params;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  #if __DEBUG_E2DBG__
  printf("[e2dbg__fpstart] Start\n");
  #endif
  
  //Find the real symbol
  orig = (eresi_Addr) e2dbg_dlsym("__fpstart");
  if (!orig)
  {
  write(1, "Error : Orig __fpstart not found\n", 33);
  return;
  }  
  realfpstart = (void *) orig;
  
  #if __DEBUG_E2DBG__
  printf("[e2dbg__fpstart] 2\n");
  #endif
  
  //Load the debugger
  argv[0] = E2DBG_ARGV0;
  argv[1] = ubp_av[0]; 
  argv[2] = NULL;
  params.ac = 2;
  params.av = argv;
  //CLRSIG;
  e2dbgworld.preloaded = 1;
  e2dbg_entry(&params); 
  //SETSIG;
  
  #if __DEBUG_E2DBG__
  printf("[e2dbg__fpstart] End \n");
  #endif
  
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
  }
*/


#else
 #warning "E2DBG Not yet implemented on this OS/Arch"
#endif
