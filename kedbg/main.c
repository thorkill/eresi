#include "kedbg.h"


/**************** Command registration ****************/
static void     kedbg_register_command(void)
{
  revm_command_add(COM1,          cmd_com1, revm_getvarparams, 0, HELPCOM1);
  revm_command_add(CMD_START,     cmd_kedbgcont, NULL, 1, HLP_START);
  revm_command_add(CMD_CONTINUE,  cmd_kedbgcont, NULL, 1, HLP_CONTINUE);
  revm_command_add(CMD_CONTINUE2, cmd_kedbgcont, NULL, 1, HLP_CONTINUE);

  /* Debugger only script commands */
  revm_command_add(CMD_MODE, cmd_mode, revm_getvarparams, 0, HLP_MODE);
  revm_command_add(CMD_BP, cmd_bp, revm_getvarparams, 1, HLP_BP);
  revm_command_add(CMD_BP2, cmd_bp, revm_getvarparams, 1, HLP_BP);
  revm_command_add(CMD_DELETE, cmd_delete, revm_getvarparams, 1, HLP_BP);
  revm_command_add(CMD_LINKMAP, cmd_linkmap, revm_getvarparams, 1, HLP_BP);
  revm_command_add(CMD_GOT, cmd_got, revm_getvarparams, 1, HLP_BP);
  revm_command_add(CMD_DUMPREGS, cmd_kedbg_dump_regs,
		   revm_getvarparams, 1, HLP_BP);

  /*   revm_command_add(CMD_STEP, cmd_step, NULL, 1, HLP_STEP); */
  /*   revm_command_add(CMD_BT2      , (void *) cmd_bt       , NULL,            1, HLP_BT); */

  /*   revm_command_add(CMD_WATCH    , (void *) cmd_watch    , revm_getvarparams, 1, HLP_WATCH); */
  /*   revm_command_add(CMD_STACK    , (void *) cmd_stack    , revm_getoption,    1, HLP_STACK); */
  /*   revm_command_add(CMD_DBGSTACK , (void *) cmd_dbgstack , revm_getoption,    1, HLP_DBGSTACK); */
  /*   revm_command_add(CMD_DUMPREGS , (void *) cmd_dumpregs , NULL,            1, HLP_DUMPREGS); */

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
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, E2DBG_HOST_GDB, ELFSH_OS_LINUX,
			   kedbg_setbp);
  e2dbg_register_delbreakhook(E2DBG_HOST_GDB, kedbg_delbp);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, E2DBG_HOST_GDB, ELFSH_OS_LINUX,
			   kedbg_getfp_ia32);
  elfsh_register_readmema(ELFSH_OS_LINUX, ELFSH_IOTYPE_GDBPROT, kedbg_readmema);
  elfsh_register_writemem(ELFSH_OS_LINUX, ELFSH_IOTYPE_GDBPROT, kedbg_writemem);
  elfsh_register_readmem(ELFSH_OS_LINUX,  ELFSH_IOTYPE_GDBPROT, kedbg_readmem);
  e2dbg_register_pregshook(ELFSH_ARCH_IA32, E2DBG_HOST_GDB, ELFSH_OS_LINUX,
			   kedbg_print_reg);
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


static void     find_linkmap(void)
{
  elfshlinkmap_t linkmap_copy;
  char          lmstring[300];
  elfshobj_t    *actual;
  char          **keys;
  int           keynbr;
  int           i;
  char		*base1;
  char		*base2;
  
  keys = hash_get_keys(&world.curjob->loaded, &keynbr);
  e2dbgworld.syms.map = kedbg_linkmap_getaddr();
  world.curjob->curfile->linkmap = e2dbgworld.syms.map;
  kedbg_readmema(NULL, (eresi_Addr)e2dbgworld.syms.map,
		 &linkmap_copy, sizeof(elfshlinkmap_t));

  /* Let's take the first entry, by following the prev. */
  while(linkmap_copy.lprev != NULL)
    kedbg_readmema(NULL, (eresi_Addr)linkmap_copy.lprev,
		   &linkmap_copy, sizeof(elfshlinkmap_t)); 
  
  /* And now, we read all the entries from the first one. */
  while(linkmap_copy.lnext != NULL)
    {
      kedbg_readmema(NULL, (eresi_Addr)linkmap_copy.lnext,
		     &linkmap_copy, sizeof(elfshlinkmap_t));
      kedbg_getstr(linkmap_copy.lname, lmstring, sizeof(lmstring));

      DEBUGMSG(fprintf(stderr, "linkmap->lnext: %p, linkmap->laddr: %#x, "
		       "lmstring: %s\n", linkmap_copy.lnext,
		       linkmap_copy.laddr, lmstring));
      
      if (lmstring != NULL && lmstring[0] != '\0')
	{
	  for (i = 0; i < keynbr; i++)
	    {
	      actual = hash_get(&world.curjob->loaded, keys[i]);
	      if (!actual)
		continue;
	      base1 = revm_basename(lmstring);
	      base2 = revm_basename(actual->name);
	      if (!strcmp(base1, base2))
		{
		  actual->rhdr.base = linkmap_copy.laddr;
		  DEBUGMSG(fprintf(stderr, "file->name: %s, is gonna take "
				   "the base addr: %#x\n\n", actual->name,
				   actual->rhdr.base));
		  break;
		}
	    }
	}
    }
  hash_free_keys(keys);
}


/**
 * Propagate the hostype and iotype to all loaded files. 
 */
static void     propagate_type(void)
{
  int           keynbr;
  char          **keys;
  int           index;
  elfshobj_t    *actual;
  
  keys = hash_get_keys(&world.curjob->loaded, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      actual = hash_get(&world.curjob->loaded, keys[index]);
      actual->hostype = E2DBG_HOST_GDB;
      actual->iotype  = ELFSH_IOTYPE_GDBPROT;
/*       if (actual != NULL) */
/* 	printf("Name: %s - %#x\n", actual->name, actual->rhdr.base); */
    }
  hash_free_keys(keys);
}


static eresi_Addr find_entrypoint(elfshobj_t *file)
{
  eresi_Addr    addr;
  asm_processor proc;
  elfshsect_t   *textsct;
/*   u_char        buf[BUFSIZ]; */
/*   unsigned      dis; */

  textsct = elfsh_get_section_by_name(file, ".text", NULL, NULL, NULL);

/*   printf("Value of textsct: %p - %#x - archtype: %d\n", textsct, */
/* 	 textsct->shdr->sh_addr, elfsh_get_archtype(file)); */
  asm_init_arch(&proc, elfsh_get_archtype(file));
  addr = textsct->shdr->sh_addr;
  /*   addr = mjr_find_main(file, */
/* 		       &proc, */
/* 		       buf, */
/* 		       BUFSIZ, */
/* 		       textsct->shdr->sh_addr, */
/* 		       &dis); */

  DEBUGMSG(fprintf(stderr, "Entry point found: %#x\n", addr));
  return addr;
}

/**
 * Shell related stuff.
 */
static int      kedbg_main(int argc, char **argv)
{
  int           ret;
  elfshbp_t     bp;
  
  /* The "1" stands for interactive. */
  revm_setup(1, argv, REVM_STATE_CMDLINE, REVM_SIDE_CLIENT);
  revm_config(".kedbgrc");
  revm_set_prompt(kedbg_create_prompt);

  //  e2dbg_register_command();
  /* Overwrite of some commands. */

  kedbg_register_command();
  hash_init(&e2dbgworld.threads, "threads", 5, ASPECT_TYPE_UNKNOW);
  hash_init(&e2dbgworld.bp, "breakpoints", 51, ASPECT_TYPE_UNKNOW);
  kedbg_curthread_init();	
  e2dbg_setup_hooks();
  kedbg_register_vector();
  ret = revm_file_load(argv[3], 0, NULL);
  ASSERT(!ret);
  kedbg_post_load_register_vector();
  elfsh_set_debug_mode();
  world.curjob->curfile->hostype = E2DBG_HOST_GDB;
  world.curjob->curfile->iotype  = ELFSH_IOTYPE_GDBPROT;
  propagate_type();

  memset(bp.savedinstr, 0, 16);
  bp.addr = find_entrypoint(world.curjob->curfile);
  /* Call the bp hook. */
  kedbg_setbp(world.curjob->curfile, &bp);
  kedbg_continue();
  kedbg_delbp(&bp);

  find_linkmap();
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
  /* Why did it stop ? */
  gdbwrap_reason_halted(gdbwrap_current_get());
  kedbg_get_regvars_ia32();
  
  kedbg_main(argc, argv);
  return 0;
}



