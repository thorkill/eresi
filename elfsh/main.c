/*
** elfsh.c for elfsh
** 
** Started on  Wed Feb 21 22:02:36 2001 mayhem
** Last update Tue Jun  3 14:17:27 2003 mayhem
*/
#include "elfsh.h"


/* The ELFsh world */
elfshworld_t	world;



/* Signal handler for SIGINT */
void		sigint_handler(int signum)
{
  if (world.state.vm_mode == ELFSH_VMSTATE_CMDLINE)
    exit(0);
}



/* Real main function */
static int      main_loop(int argc, char **argv)
{
  int		ret;

  ret = 0;
  do {

    /* Fill argv from stdin if we are in interactive mode */
    if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE)
      argv = vm_getln(&argc);

    /* Fetch the command through hash and call the command handler */
    if (vm_parseopt(argc, argv) < 0)
      {
	if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE)
	  {
	    free(argv[1]);
	    free(argv);
	    continue;
	  }
	else
	  exit(-1);
      }
  }
  while (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE);

  /* Implicit unload or save if we are not in interactive mode */
  if (world.state.vm_mode == ELFSH_VMSTATE_CMDLINE && world.current)
    ret = vm_unload_cwfiles();
  return (ret);
}




/* Open the file , get the options, and print what the user want */
int		main(int ac, char **av)
{
#if defined(USE_READLN)
  int		cmdnum;
#endif

  bzero(&world, sizeof (world));

  /* Detect the mode we are running in */
  if (ac == 1)
    world.state.vm_mode = ELFSH_VMSTATE_IMODE;
  else if (ac == 2 && av[1][0] != ELFSH_MINUS)
    {
      if (vm_openscript(av[1], av[0]) < 0)
	QUIT_ERROR(-1);
      ac = 1;
      av[1] = NULL;
      world.state.vm_mode = ELFSH_VMSTATE_SCRIPT;
    }

  /* Set the world up */
#if defined(USE_LIBASM)
  asm_init_i386(&world.proc);
#endif

  if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE)
    vm_print_banner();
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGINT, sigint_handler);
  vm_setup_hashtables();

#if __DEBUG_SIGHANDLER__
  printf("[elfsh] started !\n");
#endif

  /* Initialize readline (hash table and completion commands) */
#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_IMODE) 
    {
      world.cmds = hash_get_keys(&cmd_hash, &cmdnum);
      rl_attempted_completion_function = coustom_completion;
    }
  else
    rl_bind_key ('\t', rl_insert);
#endif

  /* Now run f0r3st */
  return (main_loop(ac, av));
}





