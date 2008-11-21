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
  revm_command_add(CMD_STEP, cmd_kedbgstep, revm_getvarparams, 1, HLP_BP);
  /* stackaddr is not set, ie does not work. */
  revm_command_add(CMD_BT, cmd_bt, NULL, 1, HLP_BT);
  revm_command_add(CMD_BT2, cmd_bt, NULL, 1, HLP_BT);
  revm_command_add(CMD_DBGSTACK, cmd_dbgstack, revm_getoption,1, HLP_DBGSTACK);
  revm_command_add(CMD_DISPLAY, cmd_display, revm_getvarparams, 1, HLP_DISPLAY);
  revm_command_add(CMD_UNDISPLAY, cmd_undisplay, revm_getvarparams, 1,
		   HLP_UNDISPLAY);
  revm_command_add(CMD_RSHT, cmd_rsht, revm_getregxoption, 1, HLP_RSHT);
  revm_command_add(CMD_RPHT, cmd_rpht, revm_getregxoption, 1, HLP_RPHT);
  revm_command_add(CMD_RPHT, cmd_quit, revm_getregxoption, 1, HLP_RPHT);
  revm_command_add(CMD_QUIT, cmd_kedbgquit, NULL, 0, HLP_QUIT);
  revm_command_add(CMD_QUIT2, cmd_kedbgquit, NULL, 0, HLP_QUIT);
/*   revm_command_add(CMD_ITRACE   , (void *) cmd_itrace   , (void *) NULL  , 1, HLP_ITRACE);   */
  /*   revm_command_add(CMD_WATCH    , (void *) cmd_watch    , revm_getvarparams, 1, HLP_WATCH); */
  /*   revm_command_add(CMD_STACK    , (void *) cmd_stack    , revm_getoption,    1, HLP_STACK); */


  /*   revm_command_add(CMD_THREADS  , (void *) cmd_threads  , revm_getvarparams, 1, HLP_THREADS); */

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
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, E2DBG_HOST_GDB, ELFSH_OS_LINUX,
			   kedbg_getfp_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, E2DBG_HOST_GDB, ELFSH_OS_LINUX,
			    kedbg_getret_ia32);

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
  gdbwrap_t     *loc = gdbwrap_current_get();
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(e2dbgthread_t), -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,key, 15, -1);
  snprintf(key, 15, "%u", (unsigned int) getpid());
  new->tid     = 0;
  new->entry   = (void *) e2dbgworld.real_main;
  new->initial = 1;
  new->context = (ucontext_t *)loc;
  time(&new->stime);
  hash_add(&e2dbgworld.threads, key, new);
  e2dbgworld.curthread = new;
  e2dbgworld.threadnbr = 1;
  //  e2dbg_stack_get(new);
  /* Let's put the stack @ 0 for now... */
  new->stackaddr = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


static void     kedbg_find_linkmap(void)
{
  elfshlinkmap_t linkmap_copy;
  elfshlinkmap_t *linkmap_addr;
  char          lmstring[300];
  elfshobj_t    *actual;
  char          **keys;
  int           keynbr;
  int           i;
  char		*base1;
  char		*base2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  keys = hash_get_keys(&world.curjob->loaded, &keynbr);
  e2dbgworld.syms.map = kedbg_linkmap_getaddr();
  world.curjob->curfile->linkmap = e2dbgworld.syms.map;
  kedbg_readmema(NULL, (eresi_Addr)e2dbgworld.syms.map,
		 &linkmap_copy, sizeof(elfshlinkmap_t));

  /* Let's take the first entry, by following the prev. */
  while (linkmap_copy.lprev != NULL)
    kedbg_readmema(NULL, (eresi_Addr)linkmap_copy.lprev,
		   &linkmap_copy, sizeof(elfshlinkmap_t)); 
  
  /* And now, we read all the entries from the first one. */
  do
    {
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
		  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
			 actual->linkmap, sizeof(linkmap_copy), );
		  memcpy(actual->linkmap, &linkmap_copy, sizeof(linkmap_copy));
		  actual->rhdr.base = linkmap_copy.laddr;
		  
		  DEBUGMSG(fprintf(stderr, "file->name: %s, is gonna take "
				   "the base addr: %#x\n\n", actual->name,
				   actual->rhdr.base));
		  break;
		}
	    }
	}
      linkmap_addr = linkmap_copy.lnext;
      kedbg_readmema(NULL, (eresi_Addr)linkmap_copy.lnext,
		     &linkmap_copy, sizeof(elfshlinkmap_t));
      
    } while (linkmap_copy.lnext != NULL);
  hash_free_keys(keys);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/**
 * Propagate the hostype and iotype to all loaded files. 
 */
static void     kedbg_propagate_type(void)
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


/**
 * If the symbol __ksymtab is found in the file we are analyzing, then
 * we are analyzing a kernel.
 */
static Bool       kedbg_file_is_kernel(elfshobj_t *file)
{
  elfshsect_t   *textsct;
  
  textsct = elfsh_get_section_by_name(file, "__ksymtab", NULL, NULL, NULL);

  if (textsct != NULL)
    return TRUE;
  else
    return FALSE;
}


static eresi_Addr kedbg_find_entrypoint(elfshobj_t *file)
{
  eresi_Addr    addr;
  asm_processor proc;
  elfshsect_t   *textsct;

  textsct = elfsh_get_section_by_name(file, ".text", NULL, NULL, NULL);
  asm_init_arch(&proc, elfsh_get_archtype(file));
  addr = textsct->shdr->sh_addr;

  DEBUGMSG(fprintf(stderr, "Entry point found: %#x\n", addr));

  return addr;
}


/**
 * The got[1] entry is filled in at runtime. The idea is to add a
 * breakpoint on the entry point, then "start" and stop when getting
 * the SIGTRAP. We don't use the cmd_bp to avoid extra messages that
 * might seem suspicious.
 */
static void     kedbg_run_to_entrypoint(elfshobj_t *file)
{

  elfshbp_t     bp;
  gdbwrap_t     *loc = gdbwrap_current_get();
  uint8_t       eip_pos;

  memset(bp.savedinstr, 0, 16);
  
  bp.addr = kedbg_find_entrypoint(file);

  kedbg_setbp(file, &bp);
  kedbg_continue();
  kedbg_delbp(&bp);
  eip_pos = offsetof(struct gdbwrap_gdbreg32, eip) / sizeof(ureg32);
  gdbwrap_writereg2(eip_pos, bp.addr, loc);
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
  hash_init(&e2dbgworld.bp, "breakpoints", 51, ASPECT_TYPE_UNKNOW);
  kedbg_curthread_init();	
  e2dbg_setup_hooks();
  kedbg_register_vector();
  ret = revm_file_load(argv[3], 0, NULL);
  ASSERT(!ret);
  kedbg_post_load_register_vector();
  elfsh_set_runtime_mode();
  e2dbg_presence_set();
  world.curjob->curfile->hostype = E2DBG_HOST_GDB;
  world.curjob->curfile->iotype  = ELFSH_IOTYPE_GDBPROT;
  kedbg_propagate_type();


  if (!kedbg_file_is_kernel(world.curjob->curfile))
    {
      kedbg_run_to_entrypoint(world.curjob->curfile);
      kedbg_find_linkmap();
    }
  else
    {
      world.curjob->curfile->rhdr.base = 0xc0000000;
    }

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



