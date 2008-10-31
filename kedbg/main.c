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
int             cmd_com1(void)
{

  void          *(*fct)();
  u_char        archtype;
  u_char        iotype;
  u_char        ostype;
  u_int         dim[2];
  vector_t      *mem;

  elfsh_register_readmem(ELFSH_OS_LINUX,  ELFSH_IOTYPE_GDBPROT, kedbg_readmem);
  archtype = elfsh_get_archtype(world.curjob->curfile);
  iotype = elfsh_get_iotype(world.curjob->curfile);
  ostype = elfsh_get_ostype(world.curjob->curfile);
  fprintf(stderr, "Value --: %d - %d - %d\n",archtype, iotype, ostype);
  fflush(stderr);
  mem = aspect_vector_get(ELFSH_HOOK_READMEM);
  dim[0] = ostype;
  dim[1] = iotype;
  fprintf(stderr, "Value2 --: %d \n", mem->arraysz);
  fflush(stderr);
  
  fct = aspect_vectors_select(mem, dim);
  fct(NULL);
  fprintf(stderr, "Value --: %d - %d\n", world.curjob->curfile->hostype, world.curjob->curfile->iotype);
  kedbg_get_regvars_ia32();
  //  fprintf(stderr, "YOUPLA: %#x\n", *kedbg_getpc_ia32());
}


/* Continue command. */
int             cmd_kedbgcont(void)
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
  revm_command_add(CMD_START,     cmd_kedbgcont, NULL, 1, HLP_START);
  revm_command_add(CMD_CONTINUE,  cmd_kedbgcont, NULL, 1, HLP_CONTINUE);
  revm_command_add(CMD_CONTINUE2, cmd_kedbgcont, NULL, 1, HLP_CONTINUE);

  /* Debugger only script commands */
  revm_command_add(CMD_MODE, cmd_mode, revm_getvarparams, 0, HLP_MODE);
  revm_command_add(CMD_BP,   cmd_bp,   revm_getvarparams, 1, HLP_BP);
  revm_command_add(CMD_BP2,  cmd_bp,   revm_getvarparams, 1, HLP_BP);

  /*   revm_command_add(CMD_STEP, cmd_step, NULL, 1, HLP_STEP); */
  /*   revm_command_add(CMD_BT2      , (void *) cmd_bt       , NULL,            1, HLP_BT); */
/*   revm_command_add(CMD_CONTINUE , (void *) cmd_cont     , (void *) NULL,   1, HLP_CONTINUE); */
/*   revm_command_add(CMD_CONTINUE2, (void *) cmd_cont     , (void *) NULL,   1, HLP_CONTINUE); */
/*   revm_command_add(CMD_START    , (void *) cmd_start    , (void *) NULL,   1, HLP_START); */

/*   revm_command_add(CMD_WATCH    , (void *) cmd_watch    , revm_getvarparams, 1, HLP_WATCH); */
/*   revm_command_add(CMD_STACK    , (void *) cmd_stack    , revm_getoption,    1, HLP_STACK); */
/*   revm_command_add(CMD_DBGSTACK , (void *) cmd_dbgstack , revm_getoption,    1, HLP_DBGSTACK); */
/*   revm_command_add(CMD_DUMPREGS , (void *) cmd_dumpregs , NULL,            1, HLP_DUMPREGS); */
/*   revm_command_add(CMD_DELETE   , (void *) cmd_delete   , revm_getoption,    1, HLP_DELETE); */

/*   revm_command_add(CMD_DISPLAY  , (void *) cmd_display  , revm_getvarparams, 1, HLP_DISPLAY); */
/*   revm_command_add(CMD_UNDISPLAY, (void *) cmd_undisplay, revm_getvarparams, 1, HLP_UNDISPLAY); */
/*   revm_command_add(CMD_RSHT     , (void *) cmd_rsht     , revm_getregxoption, 1, HLP_RSHT); */
/*   revm_command_add(CMD_RPHT     , (void *) cmd_rpht     , revm_getregxoption, 1, HLP_RPHT); */
/*   revm_command_add(CMD_THREADS  , (void *) cmd_threads  , revm_getvarparams, 1, HLP_THREADS); */
/*   revm_command_add(CMD_ITRACE   , (void *) cmd_itrace   , (void *) NULL  , 1, HLP_ITRACE); */
}


/**************** Vector initialization ****************/
static void     kedbg_register_vector(void)
{
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, E2DBG_HOST_GDB,
			   ELFSH_OS_LINUX, kedbg_bt_ia32);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, E2DBG_HOST_GDB, 
			   ELFSH_OS_LINUX, kedbg_getfp_ia32);
 
  elfsh_register_readmema(ELFSH_OS_LINUX, ELFSH_IOTYPE_GDBPROT, kedbg_readmema);
  elfsh_register_writemem(ELFSH_OS_LINUX, ELFSH_IOTYPE_GDBPROT, kedbg_writemem);
  elfsh_register_readmem(ELFSH_OS_LINUX,  ELFSH_IOTYPE_GDBPROT, kedbg_readmem);
/*   e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, E2DBG_HOST_USER,  */
/* 			   ELFSH_OS_FREEBSD, e2dbg_set_regvars_sparc32_bsd); */
}


static void     kedbg_post_load_register_vector(void)
{
  //elfsh_register_readmem(ELFSH_OS_LINUX,  ELFSH_IOTYPE_GDBPROT,  kedbg_readmem);
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
  kedbg_post_load_register_vector();
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



