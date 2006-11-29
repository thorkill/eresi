/*
** entry.c for e2dbg
**
** The debugger file for OS dependant entry points
** 
** Started on  Tue Jul 11 20:37:33 2003 mayhem
**
*/
#include "elfsh.h"




/* Our fake main function */
int			e2dbg_fake_main(int argc, char **argv, char **aux)
{
  pthread_t		dbg;
  e2dbgparams_t		params;
  char			*args[3];
  char			*pn;

  char			logbuf[BUFSIZ];
  unsigned int		len;

#if __DEBUG_E2DBG__
  int idx;
#endif

#if defined(__FreeBSD__)
  pn = __progname;
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

  if (pthread_create(&dbg, NULL, (void *) e2dbg_entry, &params))
    {
      write(2, "Unable to create debugger thread\n", 33);
      return (-1);
    }

#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] Locking ACK mutex \n", 46);
#endif

  /* Wait for the right to run */
  if (e2dbg_mutex_lock(&e2dbgworld.dbgack) < 0)
    write(1, " [*] Debuggee Cannot lock final dbgack mutex ! \n", 48);
  
#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] Calling ON_EXIT \n", 46);
#endif

  /* Initialize current thread information if we are debugging a monothread program */
  if (e2dbgworld.curthread == NULL)
    e2dbg_curthread_init();

  /* Wait for debuggee exit */
  on_exit((void *) wait4exit, NULL);
  SETSIG;

#if __DEBUG_E2DBG__
  len = snprintf(logbuf, BUFSIZ, 
		 "[(e2dbg)__libc_start_main] Calling main (%08X) with curthread = %08X (id = %u) \n", 
		 (elfsh_Addr) e2dbgworld.real_main, (elfsh_Addr) e2dbgworld.curthread, 
		 (unsigned int) getpid());
  write(1, logbuf, len);
#endif

  /* Call the original main */
  return (*e2dbgworld.real_main)(argc, argv, aux);
}



/* In case we have no symbols, we take control by this way */
#if defined(linux)
int	__libc_start_main(int (*main) (int, char **, char **aux),
			  int argc, char **ubp_av,
			  // FIXME on PPC
			  //ElfW(auxv_t) *__unbounded auxvec, 
			  void (*init) (void),
			  void (*fini) (void),
			  void (*rtld_fini) (void), 
			  void *__unbounded stack_end)
{
  elfsh_Addr		orig;
  int			(*libcstartmain)();
  int			ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] there\n", 33);
#endif

  /* Find the real symbol in libc */
  orig = (elfsh_Addr) e2dbg_dlsym("__libc_start_main");
  if (!orig)
    {
      write(1, "Error : Orig __libc_start_main not found\n", 41);
      return (-1);
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Orig __libc_start_main not found", (-1));
    }
  libcstartmain = (void *) orig;

#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] there 2\n", 35);
#endif

  /* Load the debugger in a new thread */
  e2dbgworld.preloaded = 1;
  CLRSIG;

  /* Initalize mutexes */
  e2dbg_mutex_init(&e2dbgworld.dbgsyn);
  e2dbg_mutex_init(&e2dbgworld.dbgack);
  e2dbg_mutex_init(&e2dbgworld.dbgwait);
  e2dbg_mutex_init(&e2dbgworld.dbgbp);
  if (e2dbg_mutex_lock(&e2dbgworld.dbgack) < 0)
    write(1, "Cannot lock initial dbgack mutex ! \n", 36);

#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] there 3 \n", 35);
#endif

  e2dbgworld.real_main = main;
  ret = libcstartmain(e2dbg_fake_main, argc, ubp_av, init, 
		      fini, rtld_fini, stack_end);

  /* Never reached */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

#elif defined(__FreeBSD__)

int				atexit(void (*fini)(void))
{
  elfsh_Addr			orig;
  int				(*libc_atexit)();
  char				*args[3];
  static short int		bFusible = 0;
  char				**env;
  char				*beg;
  e2dbgparams_t			params;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
#if __DEBUG_E2DBG__
  printf("[(e2dbg)atexit] there\n");
#endif

  /* Find the real symbol */
  orig = (elfsh_Addr) e2dbg_dlsym("atexit");
  if (!orig)
    {
      write(1, "Error : Orig atexit not found\n", 30);
      return (-1);
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Orig atexit not found", (-1));
    }
  libc_atexit = (void *) orig;
  
#if __DEBUG_E2DBG__
  printf("[(e2dbg)atexit 2\n");
#endif

  /* Get argv[0] */
  env = getenv("_");
  if (env)
    {
      beg = strchr(env, '=');
      if (beg)
	env = beg + 1;
    }

  /* Load the debugger only one time */
  if (bFusible == 0)
    {
      bFusible = 1;
      args[0] = E2DBG_ARGV0;
      args[1] = env; 
      args[2] = NULL;
      CLRSIG;
      e2dbgworld.preloaded = 1;
      params.ac = 2;
      params.av = args;
      e2dbg_entry(&params); 
      SETSIG;
    }
  
#if __DEBUG_E2DBG__
  printf("[(e2dbg)atexit 3]\n");
#endif
  
  /* Recall the original function */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     libc_atexit(fini));
}



#elif (defined(sun) && defined(__i386))
void			__fpstart(int argc, char**ubp_av)
{
  elfsh_Addr		orig;
  int			(*realfpstart)();
  char			*argv[3];
  e2dbgparams_t		params;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_E2DBG__
  printf("[e2dbg__fpstart] Start\n");
#endif

  /* Find the real symbol */
  orig = (elfsh_Addr) e2dbg_dlsym("__fpstart");
  if (!orig)
    {
      write(1, "Error : Orig __fpstart not found\n", 33);
      return (-1);
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Orig __fpstart not found", (-1));
    }  
  realfpstart = (void *) orig;

#if __DEBUG_E2DBG__
  printf("[e2dbg__fpstart] 2\n");
#endif

  /* Load the debugger */
  argv[0] = E2DBG_ARGV0;
  argv[1] = ubp_av[0]; 
  argv[2] = NULL;
  params.ac = 2;
  params.av = argv;
  CLRSIG;
  e2dbgworld.preloaded = 1;
  e2dbg_entry(&params); 
  SETSIG;

#if __DEBUG_E2DBG__
  printf("[e2dbg__fpstart] End \n");
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__);
}
#else
 #warning "E2DBG Not yet implemented on this OS/Arch"
#endif




/* Add an interp section to the library
** const char __invoke_dynamic_linker__[] 
** __attribute__ ((section (".interp"))) = "/lib/ld-linux.so.2";
*/

