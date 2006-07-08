/*
** signal.c for e2dbg
**
** The debugger file for signal handlers and various entry points
** 
** Started on  Tue Feb 11 21:17:33 2003 mayhem
** Last update Wed Aug 13 23:22:59 2005 mayhem
*/
#include "elfsh.h"



/* Signal handler for SIGSEGV */
void            e2dbg_sigsegv_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;
  e2dbgparams_t	params;

  CLRSIG;
  context = (ucontext_t *) pcontext;
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  vm_output(" [!] Segfault, entering E2dbg ...\n");
  params.ac = 1;
  params.av = argv;
  e2dbg_entry(&params);
  SETSIG;
}

/* Internal Signal handler for SIGSEGV */
void            e2dbg_internal_sigsegv_handler(int signum, siginfo_t *info, void *pcontext)
{
  ucontext_t	*context;

  CLRSIG;
  if (elfsh_is_prof_enable())
    elfsh_prof_disable();
  context = (ucontext_t *) pcontext;
  printf("\n [!] Segfault in E2dbg, exiting ...\n\n");
  vm_bt();
  cmd_quit();
  SETSIG;

}

/* Signal handler for SIGINT */
void            e2dbg_sigint_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  e2dbgparams_t	params;

  CLRSIG;

  /* Set all registers as variables and get PC */
  e2dbgworld.context = (ucontext_t *) pcontext;
  e2dbg_setup_hooks();
  e2dbg_getregs();
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  vm_output(" [*] Interrupted, entering E2dbg ...\n");
  params.ac = 1;
  params.av = argv;
  e2dbg_entry(&params);
  SETSIG;
}


/* Signal handler for SIGTRAP */
void            e2dbg_sigtrap_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;
  e2dbgparams_t	params;

#if __DEBUG_MUTEX__
  if (vm_dbgid_get() != pthread_self())
    vm_output(" [*] Debuggee in sigtrap handler\n");
  else 
    vm_output(" [*] Debugger in sigtrap handler\n");
#endif

  CLRSIG;
  context = (ucontext_t *) pcontext;

  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  vm_output(" [*] SigTRAP, entering E2dbg ...\n");

  params.ac = 1;
  params.av = argv;
  e2dbg_entry(&params);
  SETSIG;
}



/* Signal handler for SIGUSR1 in debugger */
void			e2dbg_sigusr1_handler(int signum)
{
  /* State variables */
  static int		past = 0;
  static int		count = 0;

  char			*argv[2];
  char			buf[32];
  elfshbp_t		*bp;
  int			prot;
  char			*name;
  int			off;
  int			ret;
  asm_instr		ptr;
  char			*s;
  elfsh_Addr		*pc; 
  u_int			bpsz;

#if __DEBUG_MUTEX__
  char			buf2[BUFSIZ];
  snprintf(buf2, BUFSIZ, " [*] SigUSR1, E2dbg in action ... (count : %u)\n", count);
  vm_output(buf2);
#endif

  /* Set all registers as variables and get PC */
  argv[0] = "e2dbg";
  argv[1] = NULL;
  e2dbg_setup_hooks();
  e2dbg_getregs(); 
  pc = e2dbg_getpc();

  /* Print variables and registers on breakpoints */
  //if (!world.state.vm_quiet)
  //cmd_vlist();

  /* Try to find the breakpoint at current instruction pointer */
  snprintf(buf, sizeof(buf), XFMT, *pc - 1);
  bp = hash_get(&e2dbgworld.bp, buf);

  /* If not found */
  if (!bp)
    {

      /* We are stepping, display  the instruction at $pc */
      if (e2dbgworld.step)
	{
	  ret = asm_read_instr(&ptr, (char *) ((elfsh_Addr) *pc), 16, 
			       &world.proc);
	  if (!ret)
	    s = "(bad)";
	  else
	    s = asm_display_instr_att(&ptr, *pc);

	  name = vm_resolve(world.curjob->current, *pc, &off);

	  /* Print the current instruction at $pc */
	  if (off)
	    printf(" [S] " XFMT " <%s + %d> %s \n", 
		   *pc, name, off, s);
	  else 
	    printf(" [S] " XFMT "<%s> %s \n", 
		   *pc, name, s);

	  vm_display(e2dbgworld.displaycmd, e2dbgworld.displaynbr);
	}
      
      /* Here starts the real stuff 
      **
      ** count == 1 -> execute restored instruction
      ** count == 2 -> restore breakpoint
      ** count >  2 -> antidebug
      */
      count++;	
      
      /* execute the previously restored instruction */
      if (count == 1 && !e2dbgworld.step)
	{
#if __DEBUG_MUTEX__
	  printf(" [*] Debuggee executes restored instruction \n");
#endif
	  return;
	}
      
      /* Suggested by andrewg, useful when debugging valgrind */
      if (count > 2 && !e2dbgworld.step)
	{
	  printf(".::- E2DBG WARNING -::.\n"
		 "Breakpoint triggered at location %08X which we don't know about.\n\n"
		 "This may be an anti-debug trick or the program could be inside another\n"
		 "debugger that uses breakpoints \n\n" 
		 "This use of e2dbg is unsupported for now, exiting .. \n\n", *pc - 1);
	  return;
	}

      /* test if the bp has not been deleted */
      snprintf(buf, sizeof(buf), XFMT, past);
      bp = hash_get(&e2dbgworld.bp, buf);

      /* Call the architecture dependant hook for putting back the breakpoint */
      if (bp)
	{
	  ret = elfsh_setbreak(bp->obj, bp);
	  if (ret < 0)
	    {
	      vm_output(" [E] Breakpoint reinsertion failed");
	      return;
	    }
	  past = NULL;
	}
      
      /* remove trace flag */	  
      if (!e2dbgworld.step)
	{
	  e2dbg_resetstep();
	  return;
	}
    }
  
  /* Breakpoint case */
  else
    {
      name = vm_resolve(world.curjob->current, (elfsh_Addr) *pc - 1, &off);
      s    = (vm_is_watchpoint(bp) ? "Watch" : "Break");
      bpsz = elfsh_get_breaksize(world.curjob->current);
      if (off)
	printf(" [*] %spoint found at " XFMT " <%s + %d> \n\n", 
	       s, *pc - bpsz, name, off);
      else 
	printf(" [*] %spoint found at " XFMT " <%s> \n\n",   
	       s, *pc - bpsz, name);
      
      *pc -= bpsz;
      prot = elfsh_munprotect(bp->obj, *pc,  bpsz);
      *(u_char *) ((elfsh_Addr) *pc) = bp->savedinstr[0];
      elfsh_mprotect(*pc, bpsz, prot);
      e2dbg_setstep();
      past             = *pc;
      count            = 0;
      e2dbgworld.curbp = bp;
      vm_display(bp->cmd, bp->cmdnbr);
    }
 
}


/* The breakpoint routine. Save registers and callback */
void			e2dbg_generic_breakpoint(int		signum, 
						 siginfo_t	*info, 
						 void		*pcontext)
{
  ucontext_t		*context;
  char			key[15];

#if __DEBUG_MUTEX__
  printf("\n [*] %s entering generic breakpoint (ID %u) \n",
	 (vm_dbgid_get() != pthread_self() ? "Debuggee" : "Debugger"), 
	 pthread_self());
#endif

  context = (ucontext_t *) pcontext;
  CLRSIG;
  e2dbgworld.context = context;

  /* We stop all threads and update the current thread information */
  e2dbgworld.stoppedpid = pthread_self();
  snprintf(key, sizeof(key), "%u", e2dbgworld.stoppedpid);
  e2dbgworld.curthread  = hash_get(&e2dbgworld.threads, key);
  e2dbg_thread_stopall();

  /* We call the debugger */
  pthread_kill(e2dbgworld.dbgpid, SIGUSR1);

  /* We are in a debugger script, continue without mutex */
  if (e2dbgworld.sourcing)
    goto nolock;

  /* The debuggee waits for a 'start/cont' command for resuming execution */
  if (e2dbg_mutex_lock(&e2dbgworld.dbgack) < 0)
    printf(" [*] Debuggee failed lock on DBGACK\n");
#if __DEBUG_MUTEX__
  else
    vm_output(" [*] Debuggee locked MUTEX-ACK and returning from sig handler \n");
#endif

 nolock:

  /* We allow the debugger to lock SYN again at next start */
  if (e2dbg_mutex_unlock(&e2dbgworld.dbgsyn) != 0)
    vm_output(" [*] Debuggee Cannot unlock dbgsyn ! \n");
#if __DEBUG_MUTEX__
  else
    vm_output(" [*] Debuggee continuing & unlocking mutex SYN -> will wait start in the future\n");
#endif
  
  SETSIG;
}


/* Our fake main function */
int			e2dbg_fake_main(int argc, char **argv, char **aux)
{
  pthread_t		dbg;
  e2dbgparams_t		params;
  char			*args[3];
  char			*pn;
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
  
  /* Recall the original function */
  SETSIG;
  
#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] Calling ON_EXIT \n", 46);
#endif

  /* Wait for debuggee exit */
  on_exit((void *) wait4exit, NULL);


#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] Calling back main \n", 46);
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
  e2dbg_mutex_init(&e2dbgworld.dbgter);
  if (e2dbg_mutex_lock(&e2dbgworld.dbgack) < 0)
    write(1, "Cannot lock initial dbgack mutex ! \n", 36);

  //__asm__(".long 0xCCCCCCCC");
  //raise(SIGSTOP);

#if __DEBUG_E2DBG__
  write(1, "[(e2dbg)__libc_start_main] there 3\n", 35);
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
