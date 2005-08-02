/*
** elfsh_etdyn.c
**
** The debugger file for signal handlers and various
** entry points
** 
** Started on  Tue Feb 11 21:17:33 2003 mayhem
** Last update Wed Jul 31 06:22:59 2005 mayhem
*/
#include "elfsh.h"


/* Library initialisation */
/* Only useful when debugger takes control by .ctors */
void		e2dbg_init(void)
{
  e2dbgworld.e2dbg_init_done = 1;

#if __DEBUG_E2DBG__
  printf("[e2dbg_init] there\n");
#endif

  /* installing sigsev handler */
  SETSIG;
}


/* Configure and launch elfsh */
int		e2dbg_entry(int ac, char **av)
{
  vm_config(ac, av);
  vm_print_banner(av[1]);

  /* XXX: not sure this test is necessary */
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
    e2dbg_setup(av[1]);
  vm_doswitch(1);
  return (vm_run(ac, av));
}


/* Temporary e2dbg entry point (hijack handler for main debugee function) */
int		e2dbg_run(int ac, char **av) 
{
  elfsh_Sym	*sym;
  char		*argv[3];
  static int	firsttime = 1;

  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  argv[2] = NULL;

#if __DEBUG_E2DBG__
  printf("[e2dbg_run] alive\n");
#endif

  INIT;

#if __DEBUG_E2DBG__
  printf("[e2dbg_run] after INIT\n");
#endif

  if (firsttime)
    {
#if __DEBUG_E2DBG__
      printf("\t[|%s|]\n", av[0]);
#endif
      argv[1] = av[0];
      firsttime = 0;
    }

  CLRSIG;
  e2dbg_entry(2, argv);

  /* Switch to obj 1 */
  if (vm_doswitch(1) < 0)
    {
#if __DEBUG_E2DBG__
      printf("[e2dbg_setup] fail to switch to obj 1\n");
#endif
      return (-1);
    }

  /* Resolve hijacked function address */
  // XXXXXXXX
  sym = elfsh_get_symbol_by_name(world.curjob->current, "old_main"); 
  
  if (!sym)
    {
#if __DEBUG_E2DBG__
      printf("[e2dbg_setup] hijacked function not found (we cant return"
	     " to hijacked function) \n");
#endif
    }
  else
    {
      e2dbgworld.e2dbg_funcret = (void *) sym->st_value;
#if __DEBUG_E2DBG__
      printf("[e2dbg_setup] hijacked function found\n");
#endif
    }

  SETSIG;

  /* Trampoline from debugger to hijacked function */
  if (e2dbgworld.e2dbg_funcret)
    {
      printf("[e2dbg_run] returning to "XFMT" \n", 
	     (elfsh_Addr) e2dbgworld.e2dbg_funcret);
      
      // We need to do ~ something like this 
      // e2dbg_funcret = hash_get(breakpoints, current)->ret;

      __asm__ volatile ("pop %%ebx	    \n"
			"mov %%ebp,%%esp  \n"
			"pop %%ebp	    \n"
			"jmp %0	    \n"
			:
			: "m" (e2dbgworld.e2dbg_funcret)
			);
    }
  else
    printf("[e2dbg_run] old_main not resolved\n");

  return (0);
}


void			e2dbg_setup(char *name)
{
  elfshsect_t		*got;
  elfsh_Addr		*linkmap_entry;
  void			*data;
  elfshlinkmap_t	*actual;

#if __DEBUG_E2DBG__
  printf("[e2dbg_setup] there\n");
#endif

  /* Load debugged file */
  if (name != NULL)
    {

      /* No need to fill ET_EXEC base addr */
      /* XXX: FIXME base addr for PIE binaries is unknown */
      if (vm_load_file(name, NULL, NULL) < 0)
	{

#if __DEBUG_E2DBG__
	  printf("[e2dbg_setup] file loading failed\n");
#endif
	  return;
	}

#if __DEBUG_E2DBG__
      printf("[e2dbg_setup] file %s loaded\n", name);
#endif

      world.curjob->current->linkmap = E2DBG_DYNAMIC_LINKMAP;
    }

  /* Switch to obj 1 */
  if (vm_doswitch(1) < 0)
    {
#if __DEBUG_E2DBG__
      printf("[e2dbg_setup] fail to switch to obj 1\n");
#endif
      return;
    }

  /* fix first file linkmap entry */
  if (world.curjob->current->linkmap == E2DBG_DYNAMIC_LINKMAP)
    {
      /* Get ALTGOT */
      got = elfsh_get_section_by_name(world.curjob->current, 
				      ELFSH_SECTION_NAME_ALTGOT, 
				      NULL, NULL, NULL);
      
#if __DEBUG_E2DBG__
      printf("[e2dbg_setup] altgot section found\n");
#endif
      
      /* Get ALTGOT entry */
      data = elfsh_get_raw(got);
      
      linkmap_entry = elfsh_get_got_entry_by_index(data, 1);
      
      world.curjob->current->linkmap = (elfshlinkmap_t *) 
	elfsh_get_got_val(linkmap_entry);
    }

  /* now load all linkmap's files */
  elfsh_set_static_mode();
  for (actual = elfsh_linkmap_get_lprev(world.curjob->current->linkmap);
       actual != NULL; 
       actual = elfsh_linkmap_get_lprev(actual))
    {
      char *name;
      name = elfsh_linkmap_get_lname(actual);

      if (name && *name)
	{
	  if (!vm_is_loaded(name))
	    {
	      if (vm_load_file(name, elfsh_linkmap_get_laddr(actual), 
			       world.curjob->current->linkmap) < 0)
		vm_output(" [EE] Loading failed");
	    }
	}      
    }

  for (actual = elfsh_linkmap_get_lnext(world.curjob->current->linkmap);
       actual != NULL; 
       actual = elfsh_linkmap_get_lnext(actual))
    {
      char *name;
      name = elfsh_linkmap_get_lname(actual);

      if (name && *name)
	{
	  if (!vm_is_loaded(name))
	    {
	      if (vm_load_file(name, elfsh_linkmap_get_laddr(actual), 
			       world.curjob->current->linkmap) < 0)
		vm_output(" [EE] Loading failed");
	    }
	}      
    }
  vm_output("\n");

  elfsh_set_debug_mode();

  vm_doswitch(1);

}


/* Signal handler for SIGSEGV */
void            e2dbg_sigsegv_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;

  CLRSIG;

  context = (ucontext_t *) pcontext;

  argv[0] = "e2dbg";
  argv[1] = NULL;

  printf("SIGSEGV cautch, entering E2dbg ...\n");

  e2dbg_entry(1, argv);

  SETSIG;

}

/* Signal handler for SIGINT */
void            e2dbg_sigint_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;

  CLRSIG;

  context = (ucontext_t *) pcontext;
  
  argv[0] = "e2dbg";
  argv[1] = NULL;

  printf("SIGINT cautch, entering E2dbg ...\n");

  e2dbg_entry(1, argv);

  SETSIG;

}


/* Signal handler for SIGTRAP */
void            e2dbg_sigtrap_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t *context;

  CLRSIG;

  context = (ucontext_t *) pcontext;

  argv[0] = "e2dbg";
  argv[1] = NULL;

  printf("SIGTRAP cautch, entering E2dbg ...\n");

  e2dbg_entry(1, argv);

  SETSIG;

}






/* The breakpoint routine. Save registers and callback */
void		e2dbg_genericbp_ia32(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;
  char		buf[32];
  elfshbp_t	*bp;
  int		prot;

  static int	past = 0;

  
  printf("ENTERED GENERICBP HANDLER\n");

  
  context = (ucontext_t *) pcontext;
  CLRSIG;
  argv[0] = "e2dbg";
  argv[1] = NULL;
  e2dbgworld.context = context;

  if (info->si_code == TRAP_TRACE)
    printf(" [*] Stepped at addr %08X ! \n", 
	   context->uc_mcontext.gregs[REG_EIP]);
  else
    e2dbg_entry(1, argv);

  /* Step or breakpoint ? */
  switch (info->si_code)
    {

      /* A breakpoint happened */
    case TRAP_BRKPT:
      snprintf(buf, 32, XFMT, context->uc_mcontext.gregs[REG_EIP] - 1);
  
      bp = hash_get(&e2dbgworld.bp, buf);
      if (!bp)
	{
	  printf("Warning: int3 triggered at location %08X which we don't know about. \n"
		 "This may be an anti-debug trick, proceed with caution. \n\n", 
		 context->uc_mcontext.gregs[REG_EIP] - 1);
	  //context->uc_mcontext.gregs[REG_EIP] -= 1;
	  return;
	}

      context->uc_mcontext.gregs[REG_EIP] -= 1;
      prot = elfsh_munprotect(bp->obj, context->uc_mcontext.gregs[REG_EIP],  1);
      *(u_char *) context->uc_mcontext.gregs[REG_EIP] = bp->savedinstr[0];
      elfsh_mprotect(context->uc_mcontext.gregs[REG_EIP], 1, prot);
      context->uc_mcontext.gregs[REG_EFL] |= 0x100;
      past = context->uc_mcontext.gregs[REG_EIP];
      break;

      /* Enable again tracing */
      /* Technique contributed by andrewg */
    case TRAP_TRACE:

      printf("Found TRAP_TRACE .. \n");

      if (past)
	{
	  vm_bp_add(past);
	  past = 0;
	  SETSIG;
	  return;
	}

      context->uc_mcontext.gregs[REG_EFL] |= 0x100;
      break;
    }

  SETSIG;
}



/* Signal handler for SIGTRAP */
/* not used */
void            e2dbg_step_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;

  CLRSIG;
  context = (ucontext_t *) pcontext;
  argv[0] = "e2dbg";
  argv[1] = NULL;

  printf(" [*] step_handler : Stepping at addr %08X \n", 
	 context->uc_mcontext.gregs[REG_EIP]);

  e2dbg_entry(1, argv);

  SETSIG;
}


/* Step command */
int		cmd_step()
{
  char		*argv[2];

  argv[0] = "e2dbg";
  argv[1] = NULL;

  printf(" [*] cmd_step : Stepping at addr %08X \n", 
	 e2dbgworld.context->uc_mcontext.gregs[REG_EIP]);

  e2dbg_entry(1, argv);

  __asm__("pushf;"
	  "popl %eax;"
	  "orl $0x100, %eax;" // 0x100 is the trace flag.
	  "push %eax;"
	  "popf;");

  //e2dbgworld.context->uc_mcontext.gregs[REG_EFL] |= 0x100;

  return (0);
}




/* Add an interp section to the library */
const char __invoke_dynamic_linker__[] 

__attribute__ ((section (".interp"))) = "/lib/ld-linux.so.2";
