#include "kedbg.h"


/**************** Command registration ****************/
void            kedbg_register_command(void)
{
  revm_command_add(COM1,          cmd_com1, revm_getvarparams, 0, HELPCOM1);
  /* These commands are overwritten. */
  revm_command_add(CMD_START,     cmd_kedbgcont, NULL, 1, HLP_START);
  revm_command_add(CMD_CONTINUE,  cmd_kedbgcont,  NULL, 1, HLP_CONTINUE);
  revm_command_add(CMD_CONTINUE2, cmd_kedbgcont,  NULL, 1, HLP_CONTINUE);
}


/**************** Vector initialization ****************/
void           kedbg_register_vector(void)
{
/*   e2dbg_register_gregshook(ELFSH_ARCH_IA32, E2DBG_HOST_PROC,  */
/* 			   ELFSH_OS_FREEBSD, e2dbg_get_regvars_ia32_bsd); */

}


/**************** Main stuff ****************/
void		kedbg_create_prompt(char *buf, unsigned int size)
{
  snprintf(buf, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   revm_colorget("%s", "pspecial", "("),
	   revm_colorget("%s", "psname" , KEDBGNAME),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "pversion", REVM_VERSION),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "prelease", REVM_RELEASE),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "pedition", REVM_EDITION),
	   revm_colorget("%s", "pspecial", "@"),
	   revm_colorget("%s", "psname", world.curjob->ws.name),
	   revm_colorget("%s", "pspecial", ")"));
  revm_endline();
}


/**
 * Shell related stuff.
 */
int             kedbg_main(int argc, char **argv)
{
  int           ret;

  /* The "1" stands for interactive. */
  revm_setup(1, argv, REVM_STATE_CMDLINE, REVM_SIDE_CLIENT);
  revm_config(".kedbgrc");
  revm_set_prompt(kedbg_create_prompt);

  e2dbg_register_command();
  /* Overwrite of some commands. */
  kedbg_register_command();
  

  ret = revm_file_load(argv[3], 0, NULL);
  ASSERT(!ret);
  elfsh_set_debug_mode();
  revm_run(argc, argv);
  fprintf(stderr, "Double mouche");
  fflush(stderr);
  return 0;
}


int             main(int argc, char **argv)
{
  int           fd;

  if (argc != 4)
    {
      printf(USAGE);
      return -1;
    }

  fd = gdbwrap_simpleconnect(argv[1], atoi(argv[2]));

  if (fd == -1)
    {
      fprintf(stderr, ERROR_CONNECTION);
      return -1;
    }

  /* Initialize and set the gdbwrap global variable. */
  gdbwrap_current_set(gdbwrap_init(fd));
  /* Let's say hello to the server, gdbstyle 8) */
  gdbwrap_hello(gdbwrap_current_get());
  
  
  kedbg_main(argc, argv);
  return 0;
}



