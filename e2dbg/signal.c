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
  e2dbgthread_t	*curthread;
  char		key[15];

  CLRSIG;

  /* Get the current thread */
  snprintf(key, sizeof(key), "%u", (unsigned int) pthread_self());
  curthread = hash_get(&e2dbgworld.threads, key);
  curthread->context = (ucontext_t *) pcontext;

  /* Set all registers as variables and get PC */
  //e2dbgworld.context = (ucontext_t *) pcontext;

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


/* Signal handler for SIGSTOP */
void            e2dbg_sigstop_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  e2dbgparams_t	params;
  e2dbgthread_t	*curthread;
  char		key[15];

  CLRSIG;

  /* Get the current thread */
  snprintf(key, sizeof(key), "%u", (unsigned int) pthread_self());
  curthread = hash_get(&e2dbgworld.threads, key);
  curthread->context = (ucontext_t *) pcontext;

#if __DEBUG_THREADS__
  printf("\n [*] SIGSTOP handler for thread %u \n", (unsigned int) pthread_self());
#endif

  /* Set all registers as variables and get PC */
  //e2dbgworld.context = (ucontext_t *) pcontext;

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

/* Signal handler for SIGUSR2 */
void            e2dbg_thread_sigusr2(int signum, siginfo_t *info, void *pcontext)
{
  e2dbgthread_t	*curthread;
  char		key[15];

  //CLRSIG;
  //usleep(100);
  e2dbgworld.curthread->state = E2DBG_THREAD_SIGUSR2;
  
  /* Get the current thread */
  snprintf(key, sizeof(key), "%u", (unsigned int) pthread_self());
  curthread = hash_get(&e2dbgworld.threads, key);
  curthread->context = (ucontext_t *) pcontext;

#if __DEBUG_THREADS__
  printf(" [*] SIGUSR2 received by thread %u \n", (unsigned int) pthread_self());
#endif
  
  e2dbgworld.threadsyncnbr++;
  e2dbgworld.curthread->state = E2DBG_THREAD_BREAKUSR2;

  pthread_kill(pthread_self(), SIGSTOP);
  //SETSIG;
}




/* Signal handler for SIGTRAP */
void            e2dbg_sigtrap_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;
  e2dbgparams_t	params;

  CLRSIG;

#if (__DEBUG_THREADS__ || __DEBUG_E2DBG__ || __DEBUG_MUTEX__)
  if (vm_dbgid_get() != pthread_self())
    vm_output(" [*] Debuggee in SIGTRAP handler\n");
  else 
    vm_output(" [*] Debugger in SIGTRAP handler\n");
#endif

  context = (ucontext_t *) pcontext;
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  vm_output(" [*] SIGTRAP : Entering E2dbg.\n");
  params.ac = 1;
  params.av = argv;
  e2dbg_entry(&params);
  SETSIG;
}



/* Signal handler for SIGUSR1 in debugger */
void			e2dbg_sigusr1_handler(int signum)
{
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
  snprintf(buf2, BUFSIZ, " [*] SigUSR1, E2dbg in action ... (count : %u)\n", 
	   e2dbgworld.curthread->count);
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
      if (e2dbgworld.curthread->step)
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
      e2dbgworld.curthread->count++;	
      
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
		 "debugger that uses breakpoints. (count = %u, step is off)\n\n" 
		 "This use of e2dbg is unsupported for now, exiting .. \n\n", 
		 e2dbgworld.curthread->count, *pc - 1);
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
	      vm_output(" [E] Breakpoint reinsertion failed");
	      return;
	    }
	  e2dbgworld.curthread->past = NULL;
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
      e2dbgworld.curthread->past             = *pc;
      e2dbgworld.curthread->count            = 0;
      e2dbgworld.curbp                       = bp;
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

  /* Do not allow processing of 2 breakpoint at the same time */
  /* We update the current thread information */
  e2dbg_mutex_lock(&e2dbgworld.dbgbp);
  
#if __DEBUG_MUTEX__
    vm_output(" [*] BP MUTEX LOCKED \n");
#endif

  e2dbgworld.stoppedpid = pthread_self();
  
#if (__DEBUG_THREADS__ || __DEBUG_E2DBG__ || __DEBUG_MUTEX__)
  printf("\n [*] %s entering generic breakpoint (ID %u) \n",
	 (vm_dbgid_get() != e2dbgworld.stoppedpid ? "Debuggee" : "Debugger"), 
	 (unsigned int) e2dbgworld.stoppedpid);
  fflush(stdout);
#endif

  snprintf(key, sizeof(key), "%u", e2dbgworld.stoppedpid);
  e2dbgworld.curthread  = hash_get(&e2dbgworld.threads, key);
  e2dbgworld.curthread->state = E2DBG_THREAD_BREAKING;
  context = (ucontext_t *) pcontext;
  e2dbgworld.curthread->context = context;

  /* We first get contexts for all other threads (except the debugger) using SIGUSR2 */
  /* Then we stop all threads */
  /* We do this only at the first state of the breakpoint */
  if (!e2dbgworld.curthread->count)
    {
      e2dbg_thread_stopall(SIGUSR2);
      while (e2dbgworld.threadnbr > e2dbgworld.threadsyncnbr + 2)
	{
#if __DEBUG_THREADS__
	  printf(" [*] Waiting for synchronization (we have %u / %u) \n", 
		 e2dbgworld.threadsyncnbr, e2dbgworld.threadnbr - 2);
#endif
	  usleep(1000);
	}
      e2dbgworld.threadsyncnbr = 0;
    }
  else
    e2dbg_thread_stopall(SIGSTOP);

  /* XXX: if the debugger is breaking , we need a new CLRSIG_BUT_USR1; macro */
  /* We call the debugger */
  pthread_kill(e2dbgworld.dbgpid, SIGUSR1);

  /* We are in a debugger script, continue without mutex */
  if (e2dbgworld.sourcing)
    goto nolock;

  /* The debuggee waits for a 'start/cont' command for resuming execution */
  if (e2dbg_mutex_lock(&e2dbgworld.dbgack) < 0)
    printf(" [*] Debuggee failed lock on dbgACK\n");
#if __DEBUG_MUTEX__
  else
    vm_output(" [*] Debuggee locked MUTEX-ACK and returning from sig handler \n");
#endif

 nolock:

  /* We allow the debugger to lock SYN again at next start */
  if (e2dbg_mutex_unlock(&e2dbgworld.dbgsyn) != 0)
    vm_output(" [*] Debuggee Cannot unlock dbgSYN ! \n");
#if __DEBUG_MUTEX__
  else
    vm_output(" [*] Debuggee continuing & unlocking mutex SYN -> will wait start in the future\n");
#endif
  
  /* Allow another breakpoint to be processed */
  if (e2dbg_mutex_unlock(&e2dbgworld.dbgbp) != 0)
    vm_output(" [*] Debuggee Cannot unlock dbgBP ! \n");
#if __DEBUG_MUTEX__
  else
    vm_output(" [*] BP MUTEX UNLOCKED \n");
#endif

  /* Continue all threads */
  if (e2dbgworld.curthread->count == 2)
    e2dbg_thread_contall();
#if __DEBUG_THREADS__
  else
    printf("Not continuing because count = %u \n", e2dbgworld.curthread->count);
#endif

  e2dbgworld.curthread->state = E2DBG_THREAD_RUNNING;
}

