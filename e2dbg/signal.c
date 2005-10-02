/*
** signal.c
**
** The debugger file for signal handlers and various
** entry points
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

  CLRSIG;
  context = (ucontext_t *) pcontext;
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  vm_output(" [!] Segfault, entering E2dbg ...\n");
  e2dbg_entry(1, argv);
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
  //ucontext_t	*context;

  CLRSIG;

  //context = (ucontext_t *) pcontext;

  /* Set all registers as variables and get PC */
  e2dbgworld.context = (ucontext_t *) pcontext;
  e2dbg_setup_hooks();
  e2dbg_getregs();

  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  vm_output(" [*] Interrupted, entering E2dbg ...\n");
  e2dbg_entry(1, argv);
  SETSIG;
}


/* Signal handler for SIGTRAP */
void            e2dbg_sigtrap_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;

  CLRSIG;
  context = (ucontext_t *) pcontext;

  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  vm_output(" [*] SigTRAP, entering E2dbg ...\n");
  e2dbg_entry(1, argv);
  SETSIG;
}


/* The breakpoint routine. Save registers and callback */
void			e2dbg_generic_breakpoint(int		signum, 
						 siginfo_t	*info, 
						 void		*pcontext)
{
  /* State variables */
  static int		past = 0;
  static elfshobj_t	*obj = NULL;
  static int		count = 0;

  char			*argv[2];
  ucontext_t		*context;
  char			buf[32];
  elfshbp_t		*bp;
  int			prot;
  char			*name;
  int			off;
  int			ret;
  asm_instr		ptr;
  char			*s;
  elfsh_Addr		*pc; 
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  context = (ucontext_t *) pcontext;
  CLRSIG;
  argv[0] = "e2dbg";
  argv[1] = NULL;
  e2dbgworld.context = context;

  /* Set all registers as variables and get PC */
  e2dbg_setup_hooks();
  e2dbg_getregs(); 

  pc = e2dbg_getpc();

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

      /*
      ** Here starts the real stuff 
      **
      ** count == 1 -> execute restored instruction
      ** count == 2 -> restore breakpoint
      ** count >  2 -> antidebug
      */
      count++;	
      
      /* execute the previously restored instruction */
      if (count == 1 && !e2dbgworld.step)
	{
	  SETSIG;
	  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
	}
      
      /* Suggested by andrewg, useful when debugging valgrind */
      if (count > 2 && !e2dbgworld.step)
	{
	  printf(".::- E2DBG WARNING -::.\n"
		 "Breakpoint triggered at location %08X which we don't know about.\n\n"
		 "This may be an anti-debug trick or the program could be inside another\n"
		 "debugger that uses breakpoints \n\n" 
		 "This use of e2dbg is unsupported for now, exiting .. \n\n", *pc - 1);
		
	  SETSIG;
	  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
	  exit(-1);
	}

      /* test if the bp has not been deleted */
      snprintf(buf, sizeof(buf), XFMT, past);
      bp = hash_get(&e2dbgworld.bp, buf);

      /* XXX: Architecture dependant, putbreak dedicated function needed */
      if (obj && bp)
	{
	  prot = elfsh_munprotect(obj, past,  1);
	  *(u_char *) past = '\xCC';
	  elfsh_mprotect(past, 1, prot);
	  past = NULL;
	  obj = NULL;
	}
      
      /* remove trace flag */	  
      if (!e2dbgworld.step)
	{
	  e2dbg_resetstep();
	  SETSIG;
	  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
	}
    }
  
  /* Breakpoint case */
  /* XXX: architecture dependant, rmbreak hook needed */
  else
    {
      name = vm_resolve(world.curjob->current, (elfsh_Addr) *pc - 1, &off);
      s = (vm_is_watchpoint(bp) ? "Watch" : "Break");
      if (off)
	printf(" [*] %spoint found at " XFMT " <%s + %d> \n\n", 
	       s, *pc - 1, name, off);
      else 
	printf(" [*] %spoint found at " XFMT " <%s> \n\n",   
	       s, *pc - 1, name);
      
      *pc -= 1;
      prot = elfsh_munprotect(bp->obj, *pc,  1);
      *(u_char *) ((elfsh_Addr) *pc) = bp->savedinstr[0];
      elfsh_mprotect(*pc, 1, prot);
      e2dbg_setstep();
      past = *pc;

      obj              = bp->obj;
      count            = 0;
      e2dbgworld.curbp = bp;
      vm_display(bp->cmd, bp->cmdnbr);
    }
  
  e2dbg_entry(1, argv);
  e2dbg_setregs();
  
  cmd_vlist();


  SETSIG;
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
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
  void			*handle;
  elfsh_Addr		orig;
  int			(*libcstartmain)();
  char			*argv[3];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if __DEBUG_E2DBG__
  printf("[(e2dbg)__libc_start_main] there\n");
#endif

  /* Grab the original function */
  handle = dlopen("/lib/libc.so.6", RTLD_NOW);
  if (!handle)
    {
      dlerror();
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Libc not found", (-1));
    }

  /* Find the real symbol in libc */
  orig = (elfsh_Addr) dlsym(handle, "__libc_start_main");
  if (!orig)
    {
      dlerror();
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Orig __libc_start_main not found", (-1));
    }
  libcstartmain = (void *) orig;

#if __DEBUG_E2DBG__
  printf("[(e2dbg)__libc_start_main] there 2\n");
#endif

#if __DEBUG_E2DBG__
  printf("[(e2dbg)__libc_start_main] there 3\n");
#endif

  /* Load the debugger */
  argv[0] = E2DBG_ARGV0;
  argv[1] = ubp_av[0]; 
  argv[2] = NULL;
  CLRSIG;
  e2dbgworld.preloaded = 1;
  e2dbg_entry(2, argv); 
  SETSIG;

#if __DEBUG_E2DBG__
  printf("[(e2dbg)__libc_start_main] there 4\n");
#endif

  /* Recall the original function */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     libcstartmain(main, argc, ubp_av, init, 
				   fini, rtld_fini, stack_end));
}

#elif (defined(sun) && defined(__i386))
void	__fpstart(int argc, char**ubp_av)
{
  void			*handle;
  elfsh_Addr		orig;
  int			(*realfpstart)();
  char			*argv[3];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_E2DBG__
  printf("[(e2dbg)__fpstart] there\n");
#endif

  /* Grab the original function */
  handle = dlopen("/lib/libc.so.1", RTLD_NOW);
  if (!handle)
    {
      dlerror();
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Libc not found", (-1));
    }

  /* Find the real symbol in libc */
  orig = (elfsh_Addr) dlsym(handle, "__fpstart");
  if (!orig)
    {
      dlerror();
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Orig __fpstart not found", (-1));
    }
  realfpstart = (void *) orig;

#if __DEBUG_E2DBG__
  printf("[(e2dbg)__fpstart] there 2\n");
#endif

#if __DEBUG_E2DBG__
  printf("[(e2dbg)__fpstart] there 3\n");
#endif

  /* Load the debugger */
  argv[0] = E2DBG_ARGV0;
  argv[1] = ubp_av[0]; 
  argv[2] = NULL;
  CLRSIG;
  e2dbgworld.preloaded = 1;
  e2dbg_entry(2, argv); 
  SETSIG;

#if __DEBUG_E2DBG__
  printf("[(e2dbg)__fpstart] there 4\n");
#endif


  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__);
}
#else
 #warning "E2DBG Not yet implemented on this OS/Arch"
#endif




/* Add an interp section to the library
const char __invoke_dynamic_linker__[] 
__attribute__ ((section (".interp"))) = "/lib/ld-linux.so.2";
*/
