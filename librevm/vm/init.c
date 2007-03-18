/*
** elfsh.c for elfsh
**
** Started on  Wed Feb 21 22:02:36 2001 mayhem
** Updated on  Tue Jun 27 23:51:04 2006 mxatone
**
** $Id: init.c,v 1.13 2007-03-18 23:11:03 thor Exp $
**
*/

#include "revm.h"


/* The ELFsh world */
revmworld_t	world;



/* Signal handler for SIGINT */
void		sigint_handler(int signum)
{
  if (world.state.vm_mode == REVM_STATE_CMDLINE)
    vm_exit(0);
}



/* The infinite main loop */
int		vm_loop(int argc, char **argv)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = 0;

  do {

    /* Come back to the loop if a script turns into interactive mode */
  reenter:

    /* Fill argv from stdin if we are in interactive mode */
    if (world.state.vm_mode != REVM_STATE_CMDLINE || 
	world.state.vm_net == 1)
      {
	if (world.state.vm_mode != REVM_STATE_SCRIPT)
	  {
	    if (vm_select() < 0)
	      {
		fprintf(stderr,"vm_select : failed \n");
		vm_exit(-1);
	      }
	  }

	/* Take a line, execute old command if void line */
	argv = vm_input(&argc);
	if (world.state.vm_mode == REVM_STATE_INTERACTIVE ||
	    world.state.vm_mode == REVM_STATE_DEBUGGER    ||
	    world.state.vm_mode == REVM_STATE_SCRIPT      ||
	    world.state.vm_net)
	  {
	    if (argv == ((char **) REVM_INPUT_VOID))
	      continue;
	  }

	/* CTRL-D -> !argv */
	if (!argv)
	  {
	    vm_output("\n");

	    /* when debugging -> back to main program */
	    if (world.state.vm_mode == REVM_STATE_DEBUGGER)
	      goto e2dbg_cleanup;

	    /* if net is enable but we are not in e2dbg -> ignore */
	    if (world.state.vm_net)
	      continue;

	    /* othewise exit */
	    break;

	  }
      }

    /* Fetch the current scripting command */
    if (vm_parseopt(argc, argv) < 0)
      {
	if (world.state.vm_mode != REVM_STATE_CMDLINE)
	  {
	    //XFREE(__FILE__, __FUNCTION__, __LINE__,argv[1]);
	    XFREE(__FILE__, __FUNCTION__, __LINE__,argv);
	    if (world.state.vm_mode != REVM_STATE_INTERACTIVE &&
		world.state.vm_mode != REVM_STATE_DEBUGGER)
	      goto end;
	  }
	else if(!world.state.vm_net)
	  vm_exit(-1);
      }

    /* Just execute one command if we are not in script mode */
    if (world.state.vm_mode != REVM_STATE_SCRIPT)
      {
	world.curjob->curcmd = world.curjob->script[0];
	switch (vm_execmd())
	  {
	  case E2DBG_SCRIPT_CONTINUE:
	    //printf(" [*] e2dbg continue from vm_execmd \n");
	    goto e2dbg_cleanup;
	  case -1:
	    profiler_error();
	  default:
	    break;
	  }
      }

    /* Quit parsing if necessary */
    if ((!world.curjob->curcmd && world.state.vm_mode == REVM_STATE_SCRIPT) ||
	(world.curjob->curcmd && world.curjob->curcmd->name &&
	 (!strcmp(world.curjob->curcmd->name, CMD_QUIT) ||
	  !strcmp(world.curjob->curcmd->name, CMD_QUIT2))))
      break;
  }
  while (world.state.vm_mode != REVM_STATE_CMDLINE || world.state.vm_net);

  /* If we are in scripting, execute commands list now */
  if (world.state.vm_mode == REVM_STATE_SCRIPT)
    {
      world.curjob->curcmd = world.curjob->script[0];
      ret = vm_execscript();
      if (ret == ELFSH_SCRIPT_STOP)
	{
	  XCLOSE(world.curjob->ws.io.input_fd, -1);
	  world.curjob->ws.io.input_fd = 0;
	  goto reenter;
	}
      else if (ret < 0)
	profiler_error();
    }

 end:
  if (!world.state.vm_quiet && world.state.vm_mode == REVM_STATE_SCRIPT)
    {
      if (ret < 0)
	vm_output("\n [E] Script execution failed \n\n");
      else
	vm_output("\n [*] Script execution ended succesfully \n\n");
    }

  /* Implicit unload or save if we are not in interactive mode */
  if (world.state.vm_mode == REVM_STATE_CMDLINE && world.curjob->current)
    ret = vm_unload_cwfiles();

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
  
  /* Clean the script machine state when a script is over */
  world.curjob->curcmd = NULL;
 e2dbg_cleanup:
  world.curjob->script[world.curjob->sourced] = NULL;
  world.curjob->lstcmd[world.curjob->sourced] = NULL;
  
    /* We arrive here when we execute a continue command from a debugger script */
 e2dbg_continue:
#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}




/* Only one time initialisations */
/* Called from CTORS */
int		vm_init()
{
 /* Must be here in case of script params presence */
  bzero(&world, sizeof (world));

  /* Set the world up */
  asm_init_i386(&world.proc);
  asm_init_sparc(&world.proc_sparc);
  vm_initio();

  /* setting libelfsh profile function */
  /* error on stdout, profile on stderr */
  profiler_install(vm_outerr, vm_output);
  return (0);
}




/* Setup ELFsh/e2dbg hash tables and structures */
int		vm_setup(int ac, char **av, char mode, char side)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Detect the mode we are running in */
  if (mode == REVM_STATE_DEBUGGER)
    {
      /* Set the mode, side, and IO of the debugger */
      world.state.vm_mode = mode;
      world.state.vm_side = side;
      vm_fifo_io(world.curjob);

#if defined(USE_READLN)
      world.curjob->ws.io.buf = NULL;
#endif
      elfsh_set_debug_mode();
    }

  /* Set REVM in static mode */
  else
    {
      elfsh_set_static_mode();

      if (ac == 1)
	world.state.vm_mode = REVM_STATE_INTERACTIVE;
      else if (vm_testscript(ac, av))
	{
	  world.state.vm_mode = REVM_STATE_SCRIPT;
	  vm_setup_hashtables();
	  if (vm_openscript(&av[1]) < 0)
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
  
  config_add_item("graph.storegepath",
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *)"~/graphs/");

  config_add_item("graph.viewcmd",
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *)"xzgv");

  config_add_item("graph.autoview",
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 1);

  if (!mjr_init_session(&world.mjr_session))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "mjollnir session can't be initialized.", -1);
  
  vm_setup_hashtables();
  elfsh_setup_hooks();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Open the .elfshrc file and execute it */
int		vm_config()
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
    vm_output("\n [*] No configuration in ~/" REVM_CONFIG " \n\n");
  done = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Interface initialisation && loop entry point */
int		vm_run(int ac, char **av)
{

  /* Do not handle signals in debugger mode */
  if (world.state.vm_mode != REVM_STATE_DEBUGGER)
    {
      signal(SIGQUIT, SIG_IGN);
      signal(SIGTERM, SIG_IGN);
      signal(SIGINT, sigint_handler);
    }

#if __DEBUG_SIGHANDLER__
  vm_output("[elfsh:main] started !\n");
#endif

#if defined(USE_READLN)
  readln_completion_install(world.state.vm_mode);
#endif
  vm_flush();
#if defined (USE_READLN)
  vm_log(vm_get_prompt());
#endif

  /* Now run f0r3st */
  return (vm_loop(ac, av));
}
