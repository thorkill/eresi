/*
** elfsh.c for elfsh
** 
** Started on  Wed Feb 21 22:02:36 2001 mayhem
*/

#include "elfsh.h"


/* The ELFsh world */
elfshworld_t	world;


/* Signal handler for SIGINT */
void		sigint_handler(int signum)
{
  if (world.state.vm_mode == ELFSH_VMSTATE_CMDLINE)
    vm_exit(0);
}



/* The infinite main loop */
int		vm_loop(int argc, char **argv)
{
  int		ret;
  char		msg[BUFSIZ];

  ret = 0;
  
  do {

    /* Fill argv from stdin if we are in interactive mode */
    if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE || world.state.vm_net == 1)
      {
          if (world.state.vm_mode != ELFSH_VMSTATE_SCRIPT)
          {
              if (vm_select() < 0)
              {
                  fprintf(stderr,"vm_select : failed \n");
                  vm_exit(-1);
              };
          }
          
          argv = vm_input(&argc);
          if (world.state.vm_mode == ELFSH_VMSTATE_IMODE     || 
	      world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER  || 
	      world.state.vm_mode == ELFSH_VMSTATE_SCRIPT    || 
	      world.state.vm_net)
	    {
	      if (argv == ((char **) ELFSH_VOID_INPUT))
		continue;
	    }

	  /* CTRL-D -> !argv*/
	  if (!argv)
	    {
	      /* when debugging -> back to main program */
	      if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
                {
                  vm_output("\n");
                  goto e2dbg_cleanup;
                }
	      
	      /* if net is enable but we are not in e2dbg -> ignore */
	      if (world.state.vm_net)
		continue;

	      /* othewise exit */
	      break;

	    }
      }

    /* Fetch the command through hash */
    if (vm_parseopt(argc, argv) < 0)
      {
          if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE)
          {
              //free(argv[1]);
              free(argv);
              if (world.state.vm_mode != ELFSH_VMSTATE_IMODE && 
		  world.state.vm_mode != ELFSH_VMSTATE_DEBUGGER)
                  goto end;
          }
          else
              if(!world.state.vm_net)
		vm_exit(-1);
      }

    /* Quit parsing if necessary */
    if (world.curjob->io.type != ELFSH_IONET && 
	world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER && 
	world.curjob->curcmd && 
	world.curjob->curcmd->name &&
	(!strcmp(world.curjob->curcmd->name, CMD_QUIT) || 
	 !strcmp(world.curjob->curcmd->name, CMD_CONTINUE)))
	{
	  goto e2dbg_cleanup;
	}
    /* Keep the parsing/executing behavior if we are not scripting */
    if (world.state.vm_mode != ELFSH_VMSTATE_SCRIPT)
      {
	world.curjob->curcmd = world.curjob->script[0];
	vm_execmd();
      }

    /* Quit parsing if necessary */
    if ((!world.curjob->curcmd && 
	 world.state.vm_mode == ELFSH_VMSTATE_SCRIPT) ||
	(world.curjob->curcmd && 
	 world.curjob->curcmd->name &&
	 (!strcmp(world.curjob->curcmd->name, CMD_QUIT) ||
	  !strcmp(world.curjob->curcmd->name, CMD_QUIT2))))
      break;
  }
  while (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE || world.state.vm_net);


  /* If we are in scripting, execute commands queue now */
  if (world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)
    {
      world.curjob->curcmd = world.curjob->script[0];
      vm_execscript();
    }
  
 end:
  if (!world.state.vm_quiet && world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)
    vm_output("\n [*] Script execution ended. \n\n");

  /* Implicit unload or save if we are not in interactive mode */
  if (world.state.vm_mode == ELFSH_VMSTATE_CMDLINE && world.curjob->current)
    ret = vm_unload_cwfiles();

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif
  return (ret);
  
 e2dbg_cleanup:
#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif
  world.curjob->script[world.curjob->sourced] = world.curjob->curcmd = NULL;
  world.curjob->lstcmd[world.curjob->sourced] = NULL;

  snprintf(msg, BUFSIZ - 1, "\n\t [..: %s returns to the grave :...] \n", vm_get_mode_name());
  vm_output_bcast(msg);
  
  return (ret);
}

/* Only one time initialisations */
int		vm_init() 
{
  /* Must be here in case of script params presence */
  bzero(&world, sizeof (world));
  hash_init(&vars_hash, 251);

  /* Set the world up */
  asm_init_i386(&world.proc);

  vm_initio();

  /* setting libelfsh profile function */
  elfsh_set_profile(vm_output);

  return (0);
}

/* Configure ELFsh/e2dbg */
int		vm_config(int ac, char **av)
{
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Detect the mode we are running in */
  if ((0 < ac) && (ac < 3) && !strncmp(av[0], E2DBG_ARGV0, 5))
    {
      /* Set the mode of e2dbg */
      world.state.vm_mode = ELFSH_VMSTATE_DEBUGGER;
      elfsh_set_debug_mode();

    }

  /* Set libelfsh in static mode */
  else
    {
      elfsh_set_static_mode();

      if (ac == 1)
	world.state.vm_mode = ELFSH_VMSTATE_IMODE;
      else if (vm_testscript(ac, av))
	{
	  if (vm_openscript(&av[1]) < 0)
	    QUIT_ERROR(-1);
	  ac = 1;
	  av[1] = NULL;
	  world.state.vm_mode = ELFSH_VMSTATE_SCRIPT;
	}
    }

  vm_setup_hashtables();
  return (0);
}

/* Interface initialisation && loop entry point */
int		vm_run(int ac, char **av)
{
#if defined(USE_READLN)
  int		cmdnum;
#endif

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Do not handle signals in debugger mode */
  if (world.state.vm_mode != ELFSH_VMSTATE_DEBUGGER)
    {
      signal(SIGQUIT, SIG_IGN);
      signal(SIGTERM, SIG_IGN);
      signal(SIGINT, sigint_handler);
    }

#if __DEBUG_SIGHANDLER__
  vm_output("[elfsh:main] started !\n");
#endif

  /* Initialize readline (hash table and completion commands) */
#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_IMODE || 
      world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
    {
      world.cmds = hash_get_keys(&cmd_hash, &cmdnum);
      /* install readline callback function */
      rl_attempted_completion_function = custom_completion;
      rl_callback_handler_install (vm_get_prompt(), vm_ln_handler);
    }
  else
    rl_bind_key ('\t', rl_insert);
#endif

  /* Now run f0r3st */
  return (vm_loop(ac, av));
}


/* The real main function */
int		vm_main(int ac, char **av)
{
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_config(ac, av);

  if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE)
    vm_print_banner(av[1]);
  
  return (vm_run(ac, av));
}


/* Open the file , get the options, and print what the user want */
int		main(int ac, char **av)
{
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  return (vm_main(ac, av));
}



