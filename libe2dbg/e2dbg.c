/*
** e2dbg.c for e2dbg
**
** Starting routines for the debugger
** 
** Started on  Fri Jun 05 15:21:56 2005 mayhem
**
*/
#include "libe2dbg.h"


/* The debugger world */
e2dbgworld_t	e2dbgworld;


/* Debugger library initialisation */
/* Only useful when debugger takes control by .ctors */
void		e2dbg_init(void)
{
  write(1, " [D] Calling DLSYM_INIT from e2dbg init !\n", 42);
  e2dbg_dlsym_init();
  e2dbg_presence_reset();
  SETSIG_USR1; 
  write(1, " [D] Finished e2dbg ctors \n", 26);
}


/* Configure and launch the VM from the debugger */
int		e2dbg_entry(e2dbgparams_t *params)
{
  int		ac;
  char		**av;
  int		ret;

#if __DEBUG_E2DBG__
  write(1, "[e2dbg_entry] CHECKPOINT 0\n", 27);
#endif

  /* Initial settings */
  ac = params->ac;
  av = params->av;
  SETSIG_USR1;
  //signal(SIGUSR1, e2dbg_sigusr1_handler);

#if __DEBUG_E2DBG__
  printf("[e2dbg_entry] CHECKPOINT 1\n");
#endif

  /* We have a debugger script pending, continue it */
  if (e2dbgworld.sourcing)
    {
      vm_execscript();
      if (e2dbgworld.sourcing)
	return (0);
    }

#if __DEBUG_E2DBG__
  printf("[e2dbg_entry] CHECKPOINT 2\n");
#endif  

  /* If the breakpoint is a watchpoint, do not call the debugger */
  /* Displays have been executed already so its no problem to ret */
  if (e2dbg_is_watchpoint(e2dbgworld.curbp))
    return (0);

#if __DEBUG_E2DBG__
  printf("[e2dbg_entry] CHECKPOINT 3\n");
#endif

  vm_setup(ac, av, REVM_STATE_DEBUGGER, REVM_SIDE_SERVER);
  
#if __DEBUG_E2DBG__
  printf("[e2dbg_entry] CHECKPOINT 4\n");
#endif

  /* Only on first execution */
  if (ac == 2)
    {
      /* Debugger only script commands */
      vm_addcmd(CMD_MODE     , (void *) cmd_mode     , vm_getvarparams, 0, HLP_MODE);
      vm_addcmd(CMD_LINKMAP  , (void *) cmd_linkmap  , NULL,            1, HLP_LINKMAP);
      vm_addcmd(CMD_BT       , (void *) cmd_bt       , NULL,            1, HLP_BT);
      vm_addcmd(CMD_BT2      , (void *) cmd_bt       , NULL,            1, HLP_BT);
      vm_addcmd(CMD_BP       , (void *) cmd_bp       , vm_getvarparams, 1, HLP_BP);
      vm_addcmd(CMD_WATCH    , (void *) cmd_watch    , vm_getvarparams, 1, HLP_WATCH);
      vm_addcmd(CMD_BP2      , (void *) cmd_bp       , vm_getvarparams, 1, HLP_BP);
      vm_addcmd(CMD_STACK    , (void *) cmd_stack    , vm_getoption,    1, HLP_STACK);
      vm_addcmd(CMD_DBGSTACK , (void *) cmd_dbgstack , vm_getoption,    1, HLP_DBGSTACK);
      vm_addcmd(CMD_DUMPREGS , (void *) cmd_dumpregs , NULL,            1, HLP_DUMPREGS);
      vm_addcmd(CMD_DELETE   , (void *) cmd_delete   , vm_getoption,    1, HLP_DELETE);
      vm_addcmd(CMD_CONTINUE , (void *) cmd_cont     , (void *) NULL,   1, HLP_CONTINUE);
      vm_addcmd(CMD_CONTINUE2, (void *) cmd_cont     , (void *) NULL,   1, HLP_CONTINUE);

      vm_addcmd(CMD_START    , (void *) cmd_start    , (void *) NULL,   1, HLP_START);
      vm_addcmd(CMD_STEP     , (void *) cmd_step     , (void *) NULL,   1, HLP_STEP);

      vm_addcmd(CMD_DISPLAY  , (void *) cmd_display  , vm_getvarparams, 1, HLP_DISPLAY);
      vm_addcmd(CMD_UNDISPLAY, (void *) cmd_undisplay, vm_getvarparams, 1, HLP_UNDISPLAY);
      vm_addcmd(CMD_RSHT     , (void *) cmd_rsht     , vm_getregxoption, 1, HLP_RSHT);
      vm_addcmd(CMD_RPHT     , (void *) cmd_rpht     , vm_getregxoption, 1, HLP_RPHT);
      vm_addcmd(CMD_THREADS  , (void *) cmd_threads  , vm_getvarparams, 1, HLP_THREADS);
    }


  if (ac == 2 && (!e2dbgworld.curthread || !e2dbgworld.curthread->step))
    vm_print_banner(av[1]);

  if (world.state.vm_mode == REVM_STATE_DEBUGGER && e2dbg_setup(av[1]) < 0)
    {
      profiler_error();
      exit(-1);
    }
  vm_doswitch(1);

#if __DEBUG_E2DBG__
  printf("[e2dbg_entry] CHECKPOINT 5\n");
#endif

  ret = vm_run(ac, av);
  return (ret);
}



/* Load all process files in the debugger */
int			e2dbg_setup(char *name)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_E2DBG__
  printf("[e2dbg_setup] Starting \n");
#endif

  e2dbg_setup_hooks();
  vm_initio();
  vm_config();
  e2dbg_load_linkmap(name);

  /* Everything was OK */
  e2dbg_output("\n");
  elfsh_set_debug_mode();
  vm_doswitch(1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

