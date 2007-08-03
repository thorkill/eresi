/**
 * @file elfsh.c
 *
 * Started on  Wed Feb 21 22:02:36 2001 jfv
 * Updated on  Tue Jun 27 23:51:04 2006 mxatone
 *
 * $Id: init.c,v 1.31 2007-08-03 11:51:00 heroine Exp $
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
 * The infinite main loop 
 */
int		revm_loop(int argc, char **argv)
{
  char		*buggyfunc;
  char		logbuf[256];
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = 0;

  do {

    /* Come back to the loop if a script turns into interactive mode */
  reenter:

    /* Fill argv from stdin if we are in interactive mode */
    if ((world.state.revm_mode != REVM_STATE_CMDLINE 
	 && world.state.revm_mode != REVM_STATE_TRACER) 
	|| world.state.revm_net == 1)
      {
	if (world.state.revm_mode != REVM_STATE_SCRIPT)
	  {
	    if (revm_select() < 0)
	      {
		fprintf(stderr,"revm_select : failed \n");
		revm_exit(-1);
	      }

	    /* If the FIFO does not exist anymore, 
	       the server has quit, so we quit too */
	    if (world.state.revm_mode == REVM_STATE_DEBUGGER && 
		(access(REVM_FIFO_S2C, F_OK) < 0 || 
		 access(REVM_FIFO_C2S, F_OK) < 0))
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);	      
	  }

	/* Take a line, execute old command if void line */
	argv = revm_input(&argc);
	if (world.state.revm_mode == REVM_STATE_INTERACTIVE ||
	    world.state.revm_mode == REVM_STATE_DEBUGGER    ||
	    world.state.revm_mode == REVM_STATE_SCRIPT      ||
	    world.state.revm_net)
	  {
	    if (argv == ((char **) REVM_INPUT_VOID))
	      continue;
	    else if (argv == ((char **) REVM_INPUT_TRANSFERED))
	      continue;
	  }

	/* CTRL-D -> !argv */
	if (!argv)
	  {
	    revm_output("\n");

	    /* when debugging -> back to main program */
	    if (world.state.revm_mode == REVM_STATE_DEBUGGER)
	      goto e2dbg_cleanup;

	    /* if net is enable but we are not in e2dbg -> ignore */
	    if (world.state.revm_net)
	      continue;

	    /* othewise exit */
	    break;

	  }
      }

    /* Fetch the current scripting command */
    if (revm_parseopt(argc, argv) < 0)
      {
	if (world.state.revm_mode != REVM_STATE_CMDLINE
	    && world.state.revm_mode != REVM_STATE_TRACER)
	  {
	    XFREE(__FILE__, __FUNCTION__, __LINE__,argv);
	    if (world.state.revm_mode != REVM_STATE_INTERACTIVE &&
		world.state.revm_mode != REVM_STATE_DEBUGGER)
	      goto end;
	  }
	else if(!world.state.revm_net)
	  revm_exit(-1);
      }

    /* Just execute one command if we are not in script mode */
    if (world.state.revm_mode != REVM_STATE_SCRIPT)
      {
	world.curjob->curcmd = world.curjob->script[0];
	switch (revm_execmd())
	  {
	  case REVM_SCRIPT_CONTINUE:
	    //printf(" [*] e2dbg continue from revm_execmd \n");
	    goto e2dbg_cleanup;
	  case REVM_SCRIPT_ERROR:
	    profiler_error();
	  default:
	    break;
	  }
      }

    /* Quit parsing if necessary */
    if ((!world.curjob->curcmd && world.state.revm_mode == REVM_STATE_SCRIPT) ||
	(world.curjob->curcmd && world.curjob->curcmd->name &&
	 (!strcmp(world.curjob->curcmd->name, CMD_QUIT) ||
	  !strcmp(world.curjob->curcmd->name, CMD_QUIT2))))
      break;
  }
  while ((world.state.revm_mode != REVM_STATE_CMDLINE
	  && world.state.revm_mode != REVM_STATE_TRACER)
	 || world.state.revm_net);

  /* If we are in scripting, execute commands list now */
  if (world.state.revm_mode == REVM_STATE_SCRIPT)
    {
      world.curjob->curcmd = world.curjob->script[0];
      ret = revm_execscript();
      if (ret == REVM_SCRIPT_STOP)
	{
	  XCLOSE(world.curjob->ws.io.input_fd, -1);
	  world.curjob->ws.io.input_fd = 0;
	  goto reenter;
	}
      else if (ret < 0)
	profiler_error();
    }

 end:
  if (!world.state.revm_quiet && world.state.revm_mode == REVM_STATE_SCRIPT)
    {
      if (ret < 0)
	revm_output("\n [E] Script execution failed \n\n");
      else
	revm_output("\n [*] Script execution ended succesfully \n\n");
    }

  /* Implicit unload or save if we are not in interactive mode */
  if ((world.state.revm_mode == REVM_STATE_CMDLINE 
       || world.state.revm_mode == REVM_STATE_TRACER) && world.curjob->current)
    {
      /* Start tracing if we are on tracer state (etrace) */
      if (world.state.revm_mode == REVM_STATE_TRACER)
	{
	  profiler_error_reset();
	  if (traces_run(world.curjob->current, NULL, 0) < 0)
	    {
	      buggyfunc = elfsh_traces_geterrfunc();
	      
	      /* Not NULL if issue occurs when we iterate though functions */
	      if (buggyfunc)
		{
		  snprintf(logbuf, 255, " [!] There is an issue with the function: %s\n",
			   buggyfunc);
		  revm_output(logbuf);
		}

	      profiler_error();
	    }
	}

      ret = revm_workfiles_unload();
    }

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
  
  /* Clean the script machine state when a script is over */
  world.curjob->curcmd = NULL;
 e2dbg_cleanup:
  world.curjob->script[world.curjob->sourced] = NULL;
  world.curjob->lstcmd[world.curjob->sourced] = NULL;

#if defined(USE_READLN)
  if (!(world.state.revm_mode == REVM_STATE_DEBUGGER
	&& world.state.revm_side == REVM_SIDE_SERVER))
    readln_quit(world.state.revm_mode);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
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
  asm_init_i386(&world.proc);
  asm_init_sparc(&world.proc_sparc);
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
int		revm_setup(int ac, char **av, char mode, char side)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Detect the mode we are running in */
  if (mode == REVM_STATE_DEBUGGER)
    {
      /* Set the mode, side, and IO of the debugger */
      world.state.revm_mode = mode;
      world.state.revm_side = side;
      revm_fifo_io(world.curjob);

#if defined(USE_READLN)
      world.curjob->ws.io.buf = NULL;
#endif
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
  config_add_item(ELFSH_VMCONFIG_ONLOAD_RCONTROL,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 1);
  
  config_add_item(ELFSH_VMCONFIG_GRAPH_STORAGEPATH,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *)"/tmp/");

  config_add_item(ELFSH_VMCONFIG_GRAPH_VIEWCMD,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *)"xzgv");

  config_add_item(ELFSH_VMCONFIG_GRAPH_AUTOVIEW,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 1);

  config_add_item(ELFSH_VMCONFIG_GRAPH_AUTOBUILD,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 1);

  config_add_item(ELFSH_VMCONFIG_USE_MORE,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 1);

  config_add_item(CONFIG_USE_ASMDEBUG,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 0);

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
int		revm_config()
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
      snprintf(buff, sizeof(buff), "%s/%s", home, REVM_CONFIG);
      XALLOC(__FILE__, __FUNCTION__, __LINE__,
	     new, sizeof(revmargv_t), -1);
      memset(new, 0, sizeof(revmargv_t));
      world.curjob->curcmd = new;
      world.curjob->curcmd->param[0] = buff;
      ret = cmd_source();
      world.curjob->curcmd = NULL;
      XFREE(__FILE__, __FUNCTION__, __LINE__,new);
    }
  if (ret < 0)
    revm_output("\n [*] No configuration in ~/" REVM_CONFIG " \n\n");
  done = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/** 
 * Interface initialisation && loop entry point 
 */
int		revm_run(int ac, char **av)
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

#if defined(USE_READLN)
  readln_completion_install(world.state.revm_mode, world.state.revm_side);
#endif
  revm_flush();
#if defined (USE_READLN)
  revm_log(revm_get_prompt());
#endif

  /* Now run f0r3st */
  return (revm_loop(ac, av));
}
