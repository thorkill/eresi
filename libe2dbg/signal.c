/*
** Signal.c for e2dbg
**
** The debugger file for signal handlers
** 
** Started on  Tue Feb 11 21:17:33 2003 mayhem
** Last update Wed Aug 13 23:22:59 2005 mayhem
**
** $Id: signal.c,v 1.5 2007-03-14 22:44:59 may Exp $
**
*/
#include "libe2dbg.h"



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

  fprintf(stderr, "\n\n ******* Segfault, entering E2dbg ******** \n\n");
  sleep(1);

  params.ac = 1;
  params.av = argv;
  e2dbg_presence_set();
  e2dbg_entry(&params);
  e2dbg_presence_reset();
  SETSIG;
}

/* Internal Signal handler for SIGSEGV */
void            e2dbg_internal_sigsegv_handler(int signum, siginfo_t *info, 
					       void *pcontext)
{
  ucontext_t	*context;

  CLRSIG;
  if (profiler_enabled())
    profiler_disable_all();
  context = (ucontext_t *) pcontext;
  printf("\n [!] Segfault in E2dbg, exiting ...\n\n");
  e2dbg_bt();
  cmd_quit();
  SETSIG;
}

/* Signal handler for SIGINT */
void            e2dbg_sigint_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  e2dbgparams_t	params;
  e2dbgthread_t	*curthread;
  char		key[15];

  CLRSIG;
  e2dbg_presence_set();

  /* Get the current thread */
  snprintf(key, sizeof(key), "%u", (unsigned int) e2dbg_self());
  curthread = hash_get(&e2dbgworld.threads, key);
  curthread->context = (ucontext_t *) pcontext;

  /* Set all registers as variables and get PC */
  //e2dbgworld.context = (ucontext_t *) pcontext;

  e2dbg_setup_hooks();
  e2dbg_getregs();
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  e2dbg_output(" [*] Interrupted, entering E2dbg ...\n");
  params.ac = 1;
  params.av = argv;
  e2dbg_entry(&params);
  e2dbg_presence_reset();
  SETSIG;
}


/* Signal handler for SIGSTOP */
void            e2dbg_sigstop_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  e2dbgparams_t	params;
  e2dbgthread_t	*curthread;
  char		key[15];

  CLRSIG;
  e2dbg_presence_set();

  /* Get the current thread */
  snprintf(key, sizeof(key), "%u", (unsigned int) e2dbg_self());
  curthread = hash_get(&e2dbgworld.threads, key);
  curthread->context = (ucontext_t *) pcontext;

#if __DEBUG_THREADS__
  printf("\n [*] SIGSTOP handler for thread %u \n", (unsigned int) e2dbg_self());
#endif

  /* Set all registers as variables and get PC */
  //e2dbgworld.context = (ucontext_t *) pcontext;

  e2dbg_setup_hooks();
  e2dbg_getregs();
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  e2dbg_output(" [*] Interrupted, entering E2dbg ...\n");
  params.ac = 1;
  params.av = argv;
  e2dbg_entry(&params);
  e2dbg_presence_reset();
  SETSIG;
}

/* Signal handler for SIGUSR2 */
void            e2dbg_thread_sigusr2(int signum, siginfo_t *info, void *pcontext)
{
  e2dbgthread_t	*curthread;
  char		key[15];
  sigset_t	mask;
  sigset_t	oldmask;
  
  e2dbgworld.curthread->state = E2DBG_THREAD_SIGUSR2;
  
  /* Get the current thread */
  snprintf(key, sizeof(key), "%u", (unsigned int) e2dbg_self());
  curthread = hash_get(&e2dbgworld.threads, key);
  curthread->context = (ucontext_t *) pcontext;

#if __DEBUG_THREADS__
  fprintf(stderr,
	  " [*] SIGUSR2 received by thread %u \n", (unsigned int) e2dbg_self());
#endif
  
  e2dbgworld.threadsyncnbr++;
  e2dbgworld.curthread->state = E2DBG_THREAD_BREAKUSR2;

  sigemptyset (&mask);
  sigaddset (&mask, SIGUSR2);
  sigprocmask (SIG_BLOCK, &mask, &oldmask);
  sigsuspend (&oldmask);
  sigprocmask (SIG_UNBLOCK, &mask, NULL);

#if 1 //__DEBUG_THREADS__
  fprintf(stderr, " [T] Getting out of SIGUSR2 handler \n");
#endif

  //pthread_kill(pthread_self(), SIGSTOP);
  //pause();
  /* Make it wait until the breakpoint is finished to process */
  //e2dbg_mutex_lock(&e2dbgworld.dbgbp);
  //e2dbg_mutex_unlock(&e2dbgworld.dbgbp);
}




/* Signal handler for SIGTRAP */
void            e2dbg_sigtrap_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;
  e2dbgparams_t	params;

  CLRSIG;
  e2dbg_presence_set();

#if (__DEBUG_THREADS__ || __DEBUG_E2DBG__ || __DEBUG_MUTEX__)
  if (!e2dbg_presence_get())
    e2dbg_output(" [*] Debuggee in SIGTRAP handler\n");
  else 
    e2dbg_output(" [*] Debugger in SIGTRAP handler\n");
#endif

  context = (ucontext_t *) pcontext;
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  e2dbg_output(" [*] SIGTRAP : Entering E2dbg.\n");
  params.ac = 1;
  params.av = argv;
  e2dbg_entry(&params);
  e2dbg_presence_reset();
  SETSIG;
}



/* Signal handler for SIGUSR1 in debugger */
void			e2dbg_do_breakpoint()
{
  char			*argv[2];
  char			buf[32];
  elfshbp_t		*bp;
  int			prot;
  char			*name;
  elfsh_SAddr		off;
  int			ret;
  asm_instr		ptr;
  char			*s;
  elfsh_Addr		*pc; 
  u_int			bpsz;
  elfshsect_t		*sect;
  elfshobj_t		*parent;
  elfsh_Sym		*sym;

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

      /* We are stepping, display the instruction at $pc */
      if (e2dbgworld.curthread->step)
	{
	  ret = asm_read_instr(&ptr, (u_char *)((elfsh_Addr) *pc), 16, &world.proc);
	  if (!ret)
	    ret++;
	  parent = e2dbg_get_parent_object((elfsh_Addr) *pc);
	  sect   = elfsh_get_parent_section(parent, (elfsh_Addr) *pc, NULL);
	  name   = vm_resolve(parent, (elfsh_Addr) *pc, &off);
	  sym    = elfsh_get_metasym_by_name(parent, name);
	  vm_display_object(sect, sym, ret, 0, off, 
			    ((elfsh_Addr) *pc), name, REVM_VIEW_DISASM);
	  e2dbg_display(e2dbgworld.displaycmd, e2dbgworld.displaynbr);
	  if (!e2dbgworld.curthread->trace)
	    e2dbg_entry(NULL);
	}
      
      /* Here starts the real stuff 
      **
      ** count == 1 -> execute restored instruction
      ** count == 2 -> restore breakpoint
      ** count >  2 -> antidebug
      */
      e2dbgworld.curthread->count++;	
      
#if __DEBUG_THREADS_
      printf(" [C] Count %u -> %u for thread ID %u \n", 
	     e2dbgworld.curthread->count - 1, 
	     e2dbgworld.curthread->count, 
	     ((unsigned int) e2dbgworld.curthread->tid));
#endif

      /* execute the previously restored instruction */
      if (e2dbgworld.curthread->count == 1 && !e2dbgworld.curthread->step)
	{
#if __DEBUG_MUTEX__
	  printf(" [*] Debuggee executes restored instruction \n");
#endif
	  return;
	}
      
      /* Suggested by andrewg, useful when debugging valgrind */
      if (e2dbgworld.curthread->count > 2 && !e2dbgworld.curthread->step)
	{
	  printf(".::- E2DBG WARNING -::.\n"
		 "Breakpoint triggered at location %08X which we don't know about.\n\n"
		 "This may be an anti-debug trick or the program could be inside another\n"
		 "debugger that uses breakpoints. (count = " UFMT ", step is off)\n\n" 
		 "This use of e2dbg is unsupported for now, exiting .. \n\n", 
		 *pc - 1, e2dbgworld.curthread->count);
	  return;
	}

      /* test if the bp has not been deleted */
      snprintf(buf, sizeof(buf), XFMT, e2dbgworld.curthread->past);
      bp = hash_get(&e2dbgworld.bp, buf);

      /* Call the architecture dependant hook for putting back the breakpoint */
      if (bp)
	{
	  ret = elfsh_setbreak(bp->obj, bp);
	  if (ret < 0)
	    {
	      e2dbg_output(" [E] Breakpoint reinsertion failed");
	      return;
	    }
	  e2dbgworld.curthread->past = 0;
	}
      
      /* remove trace flag */	  
      if (!e2dbgworld.curthread->step)
	{
	  e2dbg_resetstep();
	  return;
	}
    }
  
  /* Breakpoint case */
  else
    {
      name = vm_resolve(world.curjob->current, (elfsh_Addr) *pc - 1, &off);
      s    = (e2dbg_is_watchpoint(bp) ? "Watch" : "Break");
      bpsz = elfsh_get_breaksize(world.curjob->current);

#if __DEBUG_THREADS_
      printf(" [C] Count %u -> 0 for thread ID %u \n", 
	     e2dbgworld.curthread->count, 
	     (unsigned int) e2dbgworld.curthread->tid);
#endif      

      if (off)
	printf(" [*] %spoint found at " XFMT " <%s + " DFMT "> in thread %u \n\n", 
	       s, *pc - bpsz, name, off, (unsigned int) e2dbgworld.curthread->tid);
      else 
	printf(" [*] %spoint found at " XFMT " <%s> in thread %u \n\n",   
	       s, *pc - bpsz, name, (unsigned int) e2dbgworld.curthread->tid);
      
      *pc -= bpsz;
      prot = elfsh_munprotect(bp->obj, *pc,  bpsz);
      *(u_char *) ((elfsh_Addr) *pc) = bp->savedinstr[0];
      elfsh_mprotect(*pc, bpsz, prot);
      e2dbg_setstep();
      e2dbgworld.curthread->past             = *pc;
      e2dbgworld.curthread->count            = 0;
      e2dbgworld.curbp                       = bp;
      e2dbg_display(bp->cmd, bp->cmdnbr);
      e2dbg_entry(NULL);
    }
}


/* The breakpoint routine. Save registers and callback */
void			e2dbg_generic_breakpoint(int		signum, 
						 siginfo_t	*info, 
						 void		*pcontext)
{
  ucontext_t		*context;
  char			key[15];
  pthread_t		stopped;
  
  /* Do not allow processing of 2 breakpoint at the same time */
  /* We update the current thread information */
  e2dbg_presence_set();
  e2dbg_mutex_lock(&e2dbgworld.dbgbp);
  
#if __DEBUG_MUTEX__
  e2dbg_output("------------------------------------->\n");
  e2dbg_output(" [*] BP MUTEX LOCKED [e2dbg_generic_breakpoint] \n");
  //e2dbg_threads_print();
#endif
  
  /* Get the current thread */
  stopped = e2dbg_self();
  snprintf(key, sizeof(key), "%u", (unsigned int) stopped);
  e2dbgworld.curthread = hash_get(&e2dbgworld.threads, key);
  e2dbgworld.stoppedthread = e2dbgworld.curthread;
  
#if (__DEBUG_THREADS__ || __DEBUG_E2DBG__ || __DEBUG_MUTEX__)
  printf("\n [*] %s entering generic breakpoint (ID %u) \n",
	 (e2dbg_presence_get() ? "Debugger" : "Debuggee"), 
	 (unsigned int) e2dbgworld.stoppedthread->tid);
  fflush(stdout);
#endif

  e2dbgworld.curthread->state = E2DBG_THREAD_BREAKING;
  context = (ucontext_t *) pcontext;
  e2dbgworld.curthread->context = context;

  /* We first get contexts for all other threads (except debugger) using SIGUSR2 */
  /* Then we stop all threads */
  /* We do this only at the first state (count = 0) of the breakpoint */
  if (!e2dbgworld.curthread->count)
    {
      e2dbg_thread_stopall(SIGUSR2);
      while (e2dbgworld.threadgotnbr != e2dbgworld.threadsyncnbr)
	{
#if __DEBUG_THREADS__
	  printf(" [*] Waiting for synchronization (we have %u / %u) \n", 
		 e2dbgworld.threadsyncnbr, e2dbgworld.threadgotnbr);
	  usleep(500000);
#endif
	}
      e2dbgworld.threadgotnbr = e2dbgworld.threadsyncnbr = 0;
    }

  /* Call the real breakpoint code */
  e2dbg_do_breakpoint();
  
  /* Allow another breakpoint to be processed */
  if (e2dbg_mutex_unlock(&e2dbgworld.dbgbp) != 0)
    e2dbg_output(" [*] Debuggee Cannot unlock dbgBP ! \n");
#if __DEBUG_MUTEX__
  else
    {
      e2dbg_output(" [*] BP MUTEX UNLOCKED [e2dbg_generic_breakpoint] \n");
      //e2dbg_threads_print();
      e2dbg_output("<-------------------------------------\n");
    }
#endif

  /* Continue all threads */
  if (e2dbgworld.curthread->count == 2)
    e2dbg_thread_contall();
#if __DEBUG_THREADS__
  else
    printf("Not continuing because count = %u \n", e2dbgworld.curthread->count);
#endif

  e2dbgworld.curthread->state = E2DBG_THREAD_RUNNING;
  e2dbg_presence_reset();
}

