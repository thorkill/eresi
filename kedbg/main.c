#include "kedbg.h"

kedbgworld_t    kedbgworld;

/**************** Main stuff ****************/
static void	kedbg_create_prompt(char *buf, u_int size)
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
  XALLOC(__FILE__, __FUNCTION__, __LINE__, new, sizeof(e2dbgthread_t), -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, key, 15, -1);
  snprintf(key, 15, "%u", (u_int) getpid());
  new->tid     = 0;
  new->entry   = (void *) e2dbgworld.real_main;
  new->initial = 1;
  new->context = (ucontext_t *)loc;
  time(&new->stime);
  hash_add(&e2dbgworld.threads, key, new);
  e2dbgworld.curthread = new;
  e2dbgworld.threadnbr = 1;
  e2dbgworld.curthread->stacksize = 0xffffffff;

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
  kedbg_readmema(NULL, (eresi_Addr)(uintptr_t) e2dbgworld.syms.map,
		 &linkmap_copy, sizeof(elfshlinkmap_t));

  /* Let's take the first entry, by following the prev. */
  while (linkmap_copy.lprev != NULL)
    kedbg_readmema(NULL, (eresi_Addr)(uintptr_t) linkmap_copy.lprev,
		   &linkmap_copy, sizeof(elfshlinkmap_t));

  /* And now, we read all the entries from the first one. */
  do
    {
      /* We read 40B at a time. It'll accelerate the boot. */
      kedbg_readmema(NULL, (eresi_Addr)(uintptr_t) linkmap_copy.lname,
		     lmstring, 40);
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
      kedbg_readmema(NULL, (eresi_Addr)(uintptr_t) linkmap_copy.lnext,
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
      actual->hostype = ELFSH_HOST_GDB;
      actual->iotype  = ELFSH_IOTYPE_GDBPROT;
    }
  hash_free_keys(keys);
}


/**
 * Load the BIOS map
 */
static void	kedbg_biosmap_load()
{
  elfshobj_t    *file;

  file = elfsh_create_obj("BIOS", 0, 0x100000, EM_386, ET_EXEC,
			  ELFDATA2LSB, ELFCLASS32);
  world.curjob->curfile = file;
  hash_add(&world.curjob->loaded, file->name, file);
  file->loadtime = time(&file->loadtime);
  file->iotype   = ELFSH_IOTYPE_GDBPROT;
  file->hostype  = ELFSH_HOST_GDB;
  file->id       = ++world.state.lastid;
}

//elfshobj_t      *elfsh_create_obj(char *name, eresi_Addr start, u_int size, elfsh_Half arch, elfsh_Half type, u_char enc, u_char clas)

/**
 * Load the Embedded MEMORY  map
 */
static void	kedbg_jtagmap_load()
{
  elfshobj_t    *file;

//XXX: THIS FUNCTION IS NOT READY YET
  file = elfsh_create_obj("MEMORY", 0, 0x100000, EM_386, ET_EXEC,
			  ELFDATA2LSB, ELFCLASS32);
  world.curjob->curfile = file;
  hash_add(&world.curjob->loaded, file->name, file);
  file->loadtime = time(&file->loadtime);
  file->iotype   = ELFSH_IOTYPE_GDBPROT;
  file->hostype  = ELFSH_HOST_GDB;
  file->id       = ++world.state.lastid;
}

/**
 * If the symbol __ksymtab is found in the file we are analyzing, then
 * we are analyzing a kernel.
 */
static Bool       kedbg_file_is_kernel(elfshobj_t *file)
{
  elfshsect_t     *textsct;

  textsct = elfsh_get_section_by_name(file, "__ksymtab", NULL, NULL, NULL);
  if (textsct != NULL)
    return TRUE;
  else
    return FALSE;
}


/**
 * If the file has only one segment of 1MB, we have loaded the biosmap.
 */
static Bool     kedbg_file_is_bios(elfshobj_t *file)
{
  elfsh_Phdr	*phdr;

  phdr = elfsh_get_segment_by_type(file, PT_LOAD, 0);
  if (phdr	    == NULL		||
      phdr->p_memsz != 0x100000		||
      !elfsh_segment_is_writable(phdr)	||
      !elfsh_segment_is_readable(phdr)	||
      !elfsh_segment_is_executable(phdr))
    return FALSE;
  else
    return TRUE;
}

/**
 * If there is a memory map, we are embedded.
 */
static Bool       kedbg_is_embedded()
{
  if (kedbgworld.gdbmemap != NULL)
    return TRUE;
  else
    return FALSE;
}
/**
 * If the memory map is not empty, and the elf file has
 * a .vectors_ram section, then we are debugging embedded (JTAG).
 */
static Bool       kedbg_file_is_embedded(elfshobj_t *file)
{
  elfshsect_t     *textsct;

  if (kedbg_is_embedded())
  {
    textsct = elfsh_get_section_by_name(file, ".vectors_ram", NULL, NULL, NULL);
    if (textsct != NULL)
      return TRUE;
    else
      return FALSE;
  }
  else
    return FALSE;
}

static eresi_Addr kedbg_find_entrypoint(elfshobj_t *file)
{
  eresi_Addr      addr;
  elfshsect_t     *textsct;

  textsct = elfsh_get_section_by_name(file, ".text", NULL, NULL, NULL);
  //already initialized in librevm
  //asm_init_arch(&proc, elfsh_get_archtype(file));
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

  PROFILER_INQ();
  memset(bp.savedinstr, 0, 16);
  bp.addr = kedbg_find_entrypoint(file);
  kedbg_setbp(file, &bp);
  kedbg_continue();
  kedbg_delbp(&bp);
  eip_pos = offsetof(struct gdbwrap_gdbreg32, eip) / sizeof(ureg32);
  gdbwrap_writereg(loc, eip_pos, bp.addr);
  kedbg_get_regvars_ia32();
  PROFILER_OUTQ();
}


/**
 * There is a mapped symbol table in the kernel that we can try to map,
 * but its format is awkward. This function is never used for now
 */
#if 0
static int	kedbg_ksymtab_fixup()
{
  elfshsect_t   *ksymtab;
  elfshsect_t   *ksymtab_strings;

  PROFILER_INQ();
  ksymtab = elfsh_get_section_by_name(world.curjob->curfile, "__ksymtab",
				      NULL, NULL, NULL);
  ksymtab_strings = elfsh_get_section_by_name(world.curjob->curfile,
					      "__ksymtab_strings",
					      NULL, NULL, NULL);
  if (!ksymtab_strings || !ksymtab)
    PROFILER_ERRQ("[E] Unable to find mapped kernel symtab\n", -1);
  elfsh_set_section_type(ksymtab->shdr, SHT_DYNSYM);
  elfsh_set_section_type(ksymtab_strings->shdr, SHT_STRTAB);
  elfsh_set_section_link(ksymtab->shdr, ksymtab_strings->index);
  elfsh_set_section_link(ksymtab_strings->shdr, ksymtab->index);
  PROFILER_ROUTQ(0);
}
#endif


/**
 * Shell related stuff.
 */
static int      kedbg_main(int argc, char **argv)
{
  int           ret;
  struct        sigaction sa;

  PROFILER_INQ();
  /* The "1" stands for interactive. */
  revm_setup(1, argv, REVM_STATE_INTERACTIVE, REVM_SIDE_CLIENT);
  revm_config(".kedbgrc");
  revm_set_prompt(kedbg_create_prompt);

  /* Overwrite of some commands. */
  eresi_commands_init();
  kedbg_register_command();
  hash_init(&e2dbgworld.threads, "threads", 5, ASPECT_TYPE_UNKNOW);
  hash_init(&e2dbgworld.bp, "breakpoints", 51, ASPECT_TYPE_UNKNOW);
  kedbg_curthread_init();
  e2dbg_setup_hooks();
  kedbg_register_vector();
  ret = revm_file_load(argv[2], 0, NULL);
  if (ret)
    {
      fprintf(stderr, "Wrong file - exiting\n");
      PROFILER_ERRQ("[E] Wrong file - exiting\n", -1);
    }

  elfsh_set_runtime_mode();
  e2dbg_presence_set();
  world.curjob->curfile->hostype = ELFSH_HOST_GDB;
  world.curjob->curfile->iotype  = ELFSH_IOTYPE_GDBPROT;

  /* Put all files as being debugged within a VM */
  kedbg_propagate_type();

  /* Set as 16b code. */
  asm_ia32_switch_mode(&world.proc_ia32, INTEL_REAL);

  ret = kedbg_file_is_bios(world.curjob->curfile);
  if (!kedbg_file_is_kernel(world.curjob->curfile) && !ret && !kedbg_is_embedded())
    {
      /* The process might already be running. If the got[1] is
	 filled, we don't run to the entry point. */
      kedbgworld.state = KEDBG_USERLAND;
      if (kedbg_linkmap_getaddr() == NULL)
	kedbg_run_to_entrypoint(world.curjob->curfile);
      kedbg_find_linkmap();
    }
  else if(kedbg_is_embedded())
    {
      world.curjob->curfile->linkmap = E2DBG_ABSENT_LINKMAP;
      kedbgworld.state = KEDBG_EMBEDDED;
      if(!kedbg_file_is_embedded(world.curjob->curfile))
        kedbg_jtagmap_load();
    }
  else
    {
      char          **keys;
      elfshobj_t    *actual;
      int           keynbr;

      keys = hash_get_keys(&world.curjob->loaded, &keynbr);
      actual = hash_get(&world.curjob->loaded, keys[0]);

      world.curjob->curfile->linkmap = E2DBG_ABSENT_LINKMAP;
      kedbgworld.state = KEDBG_VM;
      kedbg_isrealmode();
      /* If we have not loaded the bios but the kernel, load the BIOS map. */
      if (!ret)
	kedbg_biosmap_load();
    }

  /* Signal handler: We want primitives to return EINTR, hence sa_flags = 0. */
  sa.sa_handler = kedbg_sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);

  revm_run_no_handler(argc, argv);
  PROFILER_ROUTQ(0);
}


int             main(int argc, char **argv)
{
  int           fd;
  char          *a;
  char          *b;
  char          memap[500];

  /* Input checks */
  if (argc != 3)
    {
      printf(USAGE);
      return -1;
    }

  a = strtok(argv[1], ":");
  b = strtok(NULL, ":");

  if (b == NULL)
    fd = gdbwrap_simpleconnect("127.0.0.1", atoi(a));
  else
    fd = gdbwrap_simpleconnect(a, atoi(b));

  if (fd == -1)
    {
      fprintf(stderr, ERROR_CONNECTION);
      return -1;
    }

  /* Initialize and set the gdbwrap global variable. */
  gdbwrap_current_set(gdbwrap_init(fd));

  /* Let's say hello to the server, gdbstyle 8) */
  gdbwrap_hello(gdbwrap_current_get());

  /* Let's get a memory map and print it */
  kedbgworld.gdbmemap = gdbwrap_memorymap_get();

  snprintf(memap, 500, "\n---MemoryMap---\n--Flash--\n-start:0x%X\n"
         "-length:%u\n-blocksize:%u\n\n--RAM--\n-start:0x%X\n-length:%u\n\n"
         "--ROM--\n-start:0x%X\n-length:%u\n",
         kedbgworld.gdbmemap->flash.start, kedbgworld.gdbmemap->flash.length,
         kedbgworld.gdbmemap->flash.blocksize, kedbgworld.gdbmemap->ram.start,
         kedbgworld.gdbmemap->ram.length, kedbgworld.gdbmemap->rom.start,
         kedbgworld.gdbmemap->rom.length);

  revm_output(memap);


  /* Why did it stop ? */
  gdbwrap_reason_halted(gdbwrap_current_get());
  kedbg_get_regvars_ia32();
  kedbg_present_set();


  return kedbg_main(argc, argv);
}



