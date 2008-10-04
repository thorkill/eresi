/**
 * @file elfsh.c
 * @ingroup librevm_vm
 * Started on  Wed Feb 21 22:02:36 2001 jfv
 * Updated on  Tue Jun 27 23:51:04 2006 mxatone
 *
 * $Id: init.c,v 1.37 2008-02-16 12:32:27 thor Exp $
 *
 */

#include "revm.h"


/* The ELFsh world */
revmworld_t	world;



/** 
 * Signal handler for SIGINT 
 */
void		sigint_handler(int signum)
{
  if (world.state.revm_mode == REVM_STATE_CMDLINE)
    revm_exit(0);
}


/**
 * Only one time initialisations
 * Called from CTORS 
 */
int		revm_init()
{

  /* Must be here in case of script params presence */
  bzero(&world, sizeof (world));

  aspect_called_ctors_inc();

  /* Set the world up */
  aspect_init();
  asm_init_arch(&world.proc, ASM_PROC_IA32);
  asm_init_arch(&world.proc_sparc, ASM_PROC_SPARC);
  asm_init_arch(&world.proc_mips, ASM_PROC_MIPS);
  revm_initio();

  /* setting libelfsh profile function */
  /* error on stdout, profile on stderr */
  profiler_install(revm_outerr, revm_output);

  if (aspect_called_ctors_finished())
    e2dbg_presence_reset();

  return (0);
}


/** 
 * Setup ELFsh/e2dbg hash tables and structures 
 */
int		revm_setup(int ac, char **av, 
			   char mode, char side)
{
  char buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Detect the mode we are running in */
  if (mode == REVM_STATE_DEBUGGER)
    {
      /* Set the mode, side, and IO of the debugger */
      world.state.revm_mode = mode;
      world.state.revm_side = side;
      revm_fifo_io(world.curjob);
      revm_buffer_reset(1);
      elfsh_set_debug_mode();
    }
  else if (mode == REVM_STATE_TRACER)
    {
      elfsh_set_static_mode();

      world.state.revm_mode = mode;
      world.state.revm_side = side;
    }
  /* Set REVM in static mode */
  else
    {
      elfsh_set_static_mode();

      if (ac == 1)
	world.state.revm_mode = REVM_STATE_INTERACTIVE;
      else if (revm_testscript(ac, av))
	{
	  world.state.revm_mode = REVM_STATE_SCRIPT;
	  revm_tables_setup();
	  if (revm_openscript(&av[1]) < 0)
	    QUIT_ERROR(-1);
	  ac = 1;
	  av[1] = NULL;
	}
    }

  /* on.load. */
  config_add_item(ERESI_VMCONFIG_ONLOAD_RCONTROL,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 1);
  
  config_add_item(ERESI_VMCONFIG_GRAPH_STORAGEPATH,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *)"/tmp/");

  config_add_item(ERESI_VMCONFIG_GRAPH_VIEWCMD,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *)"xzgv");

  config_add_item(ERESI_VMCONFIG_GRAPH_AUTOVIEW,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 1);

  config_add_item(ERESI_VMCONFIG_GRAPH_AUTOBUILD,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 1);

  config_add_item(ERESI_VMCONFIG_USE_MORE,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 1);

  config_add_item(CONFIG_USE_ASMDEBUG,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 0);

  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), "%s/%s", getenv("HOME"), ERESI_DEFAULT_HISTORY);

  config_add_item(ERESI_VMCONFIG_HISTORY,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *)buff);

  revm_tables_setup();
  elfsh_setup_hooks();
  
  if (!mjr_init_session(&world.mjr_session))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "mjollnir session can't be initialized.", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * Open the .eresirc file and execute it 
 */
int		revm_config(char *config)
{
  char		buff[BUFSIZ];
  char		*home;
  int		ret;
  static int	done = 0;
  revmargv_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (done)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  ret = -1;
  home = getenv("HOME");
  if (home)
    {
      if (!config)
	snprintf(buff, sizeof(buff), "%s/%s", home, REVM_CONFIG);
      else
	snprintf(buff, sizeof(buff), "%s/%s", home, config);
      
      XALLOC(__FILE__, __FUNCTION__, __LINE__,
	     new, sizeof(revmargv_t), -1);
      memset(new, 0, sizeof(revmargv_t));
      world.curjob->curcmd = new;
      world.curjob->curcmd->param[0] = buff;
      ret = revm_source(world.curjob->curcmd->param);
      world.curjob->curcmd = NULL;
      XFREE(__FILE__, __FUNCTION__, __LINE__,new);
    }
  if (ret < 0)
    revm_output("\n [*] No configuration in ~/" REVM_CONFIG " \n\n");
  done = 1;

  revm_init_history(world.state.revm_mode);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * Interface initialisation && loop entry point 
 */
elfshobj_t	*revm_run(int ac, char **av)
{

  /* Do not handle signals in debugger mode */
  if (world.state.revm_mode != REVM_STATE_DEBUGGER)
    {
      signal(SIGQUIT, SIG_IGN);
      signal(SIGTERM, SIG_IGN);
      signal(SIGINT, sigint_handler);
    }

#if __DEBUG_SIGHANDLER__
  revm_output("[elfsh:main] started !\n");
#endif

  revm_completion_install(world.state.revm_mode, 
			  world.state.revm_side);
  revm_flush();
  revm_prompt_log();

  /* Now run f0r3st */
  revm_loop(ac, av);

  /* Always return the current working object */
  return (world.curjob->curfile);
}
