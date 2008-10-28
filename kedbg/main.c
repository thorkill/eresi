#include "kedbg.h"


/**************** Command definition. ****************/


/* Dummy function, for compatibility with e2dbg. */
int             cmd_linkmap(void)
{
  return 0;
}


int             e2dbg_linkmap_load(char *name)
{
  NOT_USED(name);
  return 0;
}


/* Debugging purpose. */
void            cmd_com1(void)
{
  fprintf(stderr, "Value --: %d\n", world.curjob->curfile->hostype);
  kedbg_get_regvars_ia32();
  //  fprintf(stderr, "YOUPLA: %#x\n", *kedbg_getpc_ia32());
}


/* Continue command. */
void            cmd_kedbgcont(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  if (!e2dbgworld.curthread->step)
    gdbwrap_continue(loc);
  else
    gdbwrap_stepi(loc);
}


/**************** Command registration ****************/
static void     kedbg_register_command(void)
{
  revm_command_add(COM1,          cmd_com1, revm_getvarparams, 0, HELPCOM1);
  /* These commands are overwritten. */
  revm_command_add(CMD_START,     cmd_kedbgcont, NULL, 1, HLP_START);
  revm_command_add(CMD_CONTINUE,  cmd_kedbgcont, NULL, 1, HLP_CONTINUE);
  revm_command_add(CMD_CONTINUE2, cmd_kedbgcont, NULL, 1, HLP_CONTINUE);
}


/**************** Vector initialization ****************/
static void     kedbg_register_vector(void)
{
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, E2DBG_HOST_GDB,
			   ELFSH_OS_LINUX, kedbg_bt_ia32);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, E2DBG_HOST_GDB, 
			   ELFSH_OS_LINUX, kedbg_getfp_ia32);
/*   e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_USER,  */
/* 			   ELFSH_OS_FREEBSD, e2dbg_set_regvars_sparc32_bsd); */
}


/**************** Main stuff ****************/
static void	kedbg_create_prompt(char *buf, unsigned int size)
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
 * Only called when running a monothread program 
 * @return
 */
static int	kedbg_curthread_init(void)
{
  e2dbgthread_t	*new;
  char		*key;

#if __DEBUG_THREADS__
  char		logbuf[BUFSIZ];
  int		ret;
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(e2dbgthread_t), -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,key, 15, -1);
  snprintf(key, 15, "%u", (unsigned int) getpid());
  new->tid     = 0;
  new->entry   = (void *) e2dbgworld.real_main;
  new->initial = 1;
  time(&new->stime);
  hash_add(&e2dbgworld.threads, key, new);
  e2dbgworld.curthread = new;
  e2dbgworld.threadnbr = 1;
  //  e2dbg_stack_get(new);
  /* Let's put the stack @ 0 for now... */
  new->stackaddr = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Shell related stuff.
 */
static int      kedbg_main(int argc, char **argv)
{
  int           ret;

  /* The "1" stands for interactive. */
  revm_setup(1, argv, REVM_STATE_CMDLINE, REVM_SIDE_CLIENT);
  revm_config(".kedbgrc");
  revm_set_prompt(kedbg_create_prompt);

  //  e2dbg_register_command();
  /* Overwrite of some commands. */

  kedbg_register_command();
  hash_init(&e2dbgworld.threads, "threads", 5, ASPECT_TYPE_UNKNOW);
  kedbg_curthread_init();	

  e2dbg_setup_hooks();
  kedbg_register_vector();
  ret = revm_file_load(argv[3], 0, NULL);
  ASSERT(!ret);
  elfsh_set_debug_mode();
  world.curjob->curfile->hostype = E2DBG_HOST_GDB;
  world.curjob->curfile->iotype  = ELFSH_IOTYPE_GDBPROT;
  revm_run(argc, argv);

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



