/**
 * @file e2dbg.c
 *
 * Starting routines for the debugger
 * 
 * Started on  Fri Jun 05 15:21:56 2005 jfv
 *
 *
 * $Id: e2dbg.c,v 1.1 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libe2dbg.h"


/* The debugger world */
e2dbgworld_t	e2dbgworld;


void           e2dbg_register_command(void)
{
  /* Debugger only script commands */
  revm_command_add(CMD_MODE     , (void *) cmd_mode     , revm_getvarparams, 0, HLP_MODE);
  revm_command_add(CMD_LINKMAP  , (void *) cmd_linkmap  , NULL,            1, HLP_LINKMAP);
  revm_command_add(CMD_BT       , (void *) cmd_bt       , NULL,            1, HLP_BT);
  revm_command_add(CMD_BT2      , (void *) cmd_bt       , NULL,            1, HLP_BT);
  revm_command_add(CMD_BP       , (void *) cmd_bp       , revm_getvarparams, 1, HLP_BP);
  revm_command_add(CMD_WATCH    , (void *) cmd_watch    , revm_getvarparams, 1, HLP_WATCH);
  revm_command_add(CMD_BP2      , (void *) cmd_bp       , revm_getvarparams, 1, HLP_BP);
  revm_command_add(CMD_STACK    , (void *) cmd_stack    , revm_getoption,    1, HLP_STACK);
  revm_command_add(CMD_DBGSTACK , (void *) cmd_dbgstack , revm_getoption,    1, HLP_DBGSTACK);
  revm_command_add(CMD_DUMPREGS , (void *) cmd_dumpregs , NULL,            1, HLP_DUMPREGS);
  revm_command_add(CMD_DELETE   , (void *) cmd_delete   , revm_getoption,    1, HLP_DELETE);
  revm_command_add(CMD_CONTINUE , (void *) cmd_cont     , (void *) NULL,   1, HLP_CONTINUE);
  revm_command_add(CMD_CONTINUE2, (void *) cmd_cont     , (void *) NULL,   1, HLP_CONTINUE);
  revm_command_add(CMD_START    , (void *) cmd_start    , (void *) NULL,   1, HLP_START);
  revm_command_add(CMD_STEP     , (void *) cmd_step     , (void *) NULL,   1, HLP_STEP);
  revm_command_add(CMD_DISPLAY  , (void *) cmd_display  , revm_getvarparams, 1, HLP_DISPLAY);
  revm_command_add(CMD_UNDISPLAY, (void *) cmd_undisplay, revm_getvarparams, 1, HLP_UNDISPLAY);
  revm_command_add(CMD_RSHT     , (void *) cmd_rsht     , revm_getregxoption, 1, HLP_RSHT);
  revm_command_add(CMD_RPHT     , (void *) cmd_rpht     , revm_getregxoption, 1, HLP_RPHT);
  revm_command_add(CMD_THREADS  , (void *) cmd_threads  , revm_getvarparams, 1, HLP_THREADS);
  revm_command_add(CMD_ITRACE   , (void *) cmd_itrace   , (void *) NULL  , 1, HLP_ITRACE);
}


/** 
 * Configure and launch the VM from the debugger.
 * @param params
 * @return
 */
int		e2dbg_entry(e2dbgparams_t *params)
{
  int		ac;
  char		**av;
  int		ret;
  static int	first = 1;

#if __DEBUG_E2DBG__
  write(2, "[e2dbg_entry] CHECKPOINT 0\n", 27);
#endif

  /* Initial settings */
  ac = (params ? params->ac : 0);
  av = (params ? params->av : NULL);
  CLRSIG;

#if __DEBUG_E2DBG__
  fprintf(stderr, "[e2dbg_entry] CHECKPOINT 1\n");
#endif

  /* We have a debugger script pending, continue it */
  if (e2dbgworld.sourcing)
    {
      fprintf(stderr, "e2dbg_entry called with SOURCED program ! calling execscript \n");

      revm_execscript();
      if (e2dbgworld.sourcing)
	return (0);
    }

#if __DEBUG_E2DBG__
  fprintf(stderr, "[e2dbg_entry] CHECKPOINT 2\n");
#endif  

  /* If the breakpoint is a watchpoint, do not call the debugger */
  /* Displays have been executed already so its no problem to ret */
  if (e2dbg_is_watchpoint(e2dbgworld.curbp))
    return (0);

#if __DEBUG_E2DBG__
  fprintf(stderr, "[e2dbg_entry] CHECKPOINT 3\n");
#endif

  if (av && ac)
    revm_setup(ac, av, REVM_STATE_DEBUGGER, REVM_SIDE_SERVER);

#if __DEBUG_E2DBG__
  fprintf(stderr, "[e2dbg_entry] CHECKPOINT 4\n");
#endif

  /* Only on first execution */
  if (first)
    {
      e2dbg_register_command();
#if __DEBUG_E2DBG__
      fprintf(stderr, "[e2dbg_entry] CHECKPOINT 4bis\n");
#endif
      
      if (!e2dbgworld.curthread || !e2dbgworld.curthread->step)
	revm_banner_print(av[1]);
      
      first = 0;
    }

#if __DEBUG_E2DBG__
  fprintf(stderr, "[e2dbg_entry] CHECKPOINT 5\n");
#endif

  if (world.state.revm_mode == REVM_STATE_DEBUGGER && av && 
      e2dbg_setup(av[1]) < 0)
    {
      profiler_error();
      exit(-1);
    }
  revm_doswitch(1);

#if __DEBUG_E2DBG__
  fprintf(stderr, "[e2dbg_entry] CHECKPOINT 6\n");
#endif

  ret = revm_run(ac, av);
  SETSIG;
  return (ret);
}



/**
 * Load all process files in the debugger 
 * @param name
 * @return
 */
int			e2dbg_setup(char *name)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_E2DBG__
  fprintf(stderr, "[e2dbg_setup] Starting \n");
#endif

  e2dbg_setup_hooks();
  revm_initio();
  revm_config(E2DBG_CONFIG);
  e2dbg_linkmap_load(name);

  /* Everything was OK */
  e2dbg_output("\n");
  elfsh_set_debug_mode();
  revm_doswitch(1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

