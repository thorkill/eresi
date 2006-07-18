/*
** resolv.c for e2dbg
**
** Contains the linkmap related resolving builtins in e2dbg
**
** Last Update Thu Oct 13 19:37:26 2005 mm
*/
#include "elfsh.h"

/* Reference symbol for the debugger */
int			reference = 42;

/* Load linkmap in PIE based process */
static int		e2dbg_load_linkmap_pie(char *name)
{
  /*
  elfshobj_t		*handle;
  elfshsect_t		*sct;
  u_int			bindnum;
  u_int			typenum;
  elfsh_Phdr		*gotseg;
  u_int			num;
  elfsh_Sym		*dynsymtab;
  elfsh_Sym		*table;
  char			*symname;
  void			*symaddr;
  elfshsect_t		*got;
  elfsh_Phdr		*sctseg;
  elfsh_Addr		symvalue;
  u_int			index;
  elfsh_Sym		*mainsym;
  elfsh_Addr		base;
  */

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  elfsh_set_static_mode();  

  /* replace by self dlopen technique */
  /*
  handle = e2dbg_dlopen(name, (elfsh_Addr) main, "main");
  if (!handle)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "e2dbg_dlopen failed", -1);    
  */

  /*
  dynsymtab = elfsh_get_dynsymtab(world.curjob->current, &num);
  if (!dynsymtab)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get dynsymtab", -1);    
      
  sct = elfsh_get_section_by_name(world.curjob->current, 
				  ELFSH_SECTION_NAME_ALTDYNSYM, 
				  NULL, NULL, &num);
  
  if (!sct)
    sct = elfsh_get_section_by_type(world.curjob->current, SHT_DYNSYM, 
				    NULL, NULL, NULL, &num);
  
  if (!sct)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get dynsym section", -1);    
  
  num = num / sizeof(elfsh_Sym);
  
  table = (elfsh_Sym *) (sct->shdr->sh_addr ? elfsh_get_raw(sct) : sct->data);
  if (!table)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get dynsym data", -1);     
  
  gotseg = elfsh_get_parent_segment(world.curjob->current, got);
  
  for (index = 0; index < num; index++)
    {
      typenum = elfsh_get_symbol_type(table + index);
      bindnum = elfsh_get_symbol_bind(table + index);
      symname = elfsh_get_dynsymbol_name(world.curjob->current, table + index);
      
      sct = elfsh_get_parent_section(world.curjob->current, 
				     table[index].st_value, 
				     NULL);
      
      if (sct == NULL && table[index].st_shndx)
	sct = elfsh_get_section_by_index(world.curjob->current, 
					 table[index].st_shndx,
					 NULL, NULL);
      
      if (symname != NULL && *symname)
	{
	  symaddr = e2dbg_dlsym(handle, symname);
	  sctseg = elfsh_get_parent_segment(world.curjob->current, sct);
	  symvalue = elfsh_get_symbol_value(table + index);
	  
	  if (symaddr && sctseg == gotseg && symvalue)
	    {
	      world.curjob->current->rhdr.base = (elfsh_Addr) symaddr - symvalue;
	      break;
	    }
	}
      
    }
  */

  /*  
  if (!handle->rhdr.base)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Base address not found", -1);    
  */

  elfsh_set_debug_mode();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Load linkmap */
int			e2dbg_load_linkmap(char *name)
{
  static int		done	= 0;
  elfshsect_t		*got;
  elfsh_Addr		*linkmap_entry;
  void			*data;
  elfshlinkmap_t	*actual;
  char			*gotname;
  char			*ename;
  elfsh_Ehdr		*hdr;
  u_int			elftypenum;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (done)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);    

  e2dbg_setup_hooks();
  vm_config();

  /* Load debugged file */
  if (name != NULL)
    {

      /* No need to fill ET_EXEC base addr */
      if (!vm_is_loaded(name) && vm_load_file(name, NULL, NULL) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot load file", -1);
      
#if __DEBUG_LINKMAP__
      printf("[e2dbg_setup] file %s loaded\n", name);
#endif

      world.curjob->current->linkmap = E2DBG_DYNAMIC_LINKMAP;
      world.curjob->current->running = 0;
    }

  /* Switch to obj 1 */
  if (vm_doswitch(1) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot switch on object 1", -1);    
  
  /* Get ALTGOT or GOT if we used LD_PRELOAD */
  if (!e2dbgworld.preloaded)
    {
      gotname = ELFSH_SECTION_NAME_ALTGOT;
      got = elfsh_get_section_by_name(world.curjob->current, 
				      gotname, NULL, NULL, NULL);
    }
  else
    got = elfsh_get_gotsct(world.curjob->current);
  
  
#if __DEBUG_LINKMAP__
  printf("[e2dbg_setup] %s section at " XFMT "\n",
	 got->name, (elfsh_Addr) got);
#endif
  
  
  /* Fix first file linkmap entry */
  if (world.curjob->current->linkmap == E2DBG_DYNAMIC_LINKMAP)
    {
      /* Fix first file linkmap entry */
      hdr = elfsh_get_hdr(world.curjob->current);
      if (!hdr)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot get ELF header", -1);    
      elftypenum = elfsh_get_objtype(hdr);
      
      /* Find the PIE binary linkmap */
      if (elftypenum == ET_DYN && e2dbg_load_linkmap_pie(name) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot get PIE linkmap", -1);    
      
      /* Get ALTGOT entry */
      data          = elfsh_get_raw(got);
      linkmap_entry = elfsh_get_got_entry_by_index(data, 1);
      
#if defined(__FreeBSD__)
      world.curjob->current->linkmap = &((Obj_Entry *) elfsh_get_got_val(linkmap_entry))->linkmap;
#else
      world.curjob->current->linkmap = (elfshlinkmap_t *) elfsh_get_got_val(linkmap_entry);
#endif
      
    }
  
#if __DEBUG_LINKMAP__
  printf("[e2dbg_setup] LINKMAP Found at " XFMT "\n", 
	 world.curjob->current->linkmap);
#endif
  
  vm_doswitch(1);
  elfsh_set_static_mode();
  
  /* now load all linkmap's files */
  for (actual = elfsh_linkmap_get_lprev(world.curjob->current->linkmap);
       actual != NULL; 
       actual = elfsh_linkmap_get_lprev(actual))
    {
      
#if __DEBUG_LINKMAP__
      printf("[e2dbg_setup] Running on LINKMAP PREV " XFMT "\n", 
	     actual);
#endif
      
      ename = elfsh_linkmap_get_lname(actual);
      if (ename && *ename && !vm_is_loaded(ename))
	{
	  if (vm_load_file(ename, elfsh_linkmap_get_laddr(actual), 
			   world.curjob->current->linkmap) < 0)
	    vm_output(" [EE] Loading failed");
	}      
    }

#if __DEBUG_LINKMAP__
  printf("[e2dbg_setup] Running on LINKMAP NEXT\n");
#endif
  
  for (actual = elfsh_linkmap_get_lnext(world.curjob->current->linkmap);
       actual != NULL; 
       actual = elfsh_linkmap_get_lnext(actual))
    {
     
#if __DEBUG_LINKMAP__
	  printf("[e2dbg_setup] Running on LINKMAP NEXT " XFMT "\n", 
		 actual);
#endif

      ename = elfsh_linkmap_get_lname(actual);
      if (ename && *ename && !vm_is_loaded(ename))
	{
	  if (vm_load_file(ename, elfsh_linkmap_get_laddr(actual), 
			   world.curjob->current->linkmap) < 0)
	    vm_output(" [EE] Loading failed");
	}      
    }

  /* Everything was OK */
  vm_output("\n");
  elfsh_set_debug_mode();
  vm_doswitch(1);
  done = 1;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Our own dlsect without malloc */
/* Useful in the early stage of mapping when malloc symbol is not yet known */
elfsh_Addr		e2dbg_dlsect(char *objname, char *sect2resolve, 
				     elfsh_Addr refaddr, char *refsym)
{
  e2dbgobj_t		obj;
  elfsh_Phdr		*pht;
  elfsh_Dyn		*dyn;
  u_int			nbr, nbr2;
  elfsh_Sym		cursym;
  char			*strtab;
  elfsh_Addr		got;
  u_int			curoff;
  elfsh_Addr		found_ref = 0;

#if __DEBUG_E2DBG__
  char		buf[BUFSIZ];
  u_int		len;
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_E2DBG__
  write(1, " [*] e2dbg_dlsect called for resolving ", 39);
  write(1, sect2resolve, strlen(sect2resolve));
  write(1, " in ", 4);
  write(1, objname, strlen(objname));
  write(1, "\n", 1);
#endif

  memset(&obj, 0x00, sizeof(obj));
  XOPEN(obj.fd, objname, O_RDONLY, 0, NULL);
  XREAD(obj.fd, &obj.e, sizeof(elfsh_Ehdr), NULL);

#if __DEBUG_E2DBG__
  write(1, " [*] 1", 6);
#endif

  XSEEK(obj.fd, obj.e.e_phoff, SEEK_SET, NULL);
  pht = alloca(obj.e.e_phnum * sizeof(elfsh_Phdr));

#if __DEBUG_E2DBG__
  write(1, "2", 1);
#endif

  XREAD(obj.fd, pht, obj.e.e_phnum * sizeof(elfsh_Phdr), NULL);
  for (nbr = 0; nbr < obj.e.e_phnum; nbr++)
    if (pht[nbr].p_type == PT_DYNAMIC)
      {
	obj.dynoff = pht[nbr].p_offset;
	break;
      }

#if __DEBUG_E2DBG__
  write(1, "3", 1);
#endif

  XSEEK(obj.fd, obj.dynoff, SEEK_SET, NULL);
  dyn = alloca(pht[nbr].p_filesz);

#if __DEBUG_E2DBG__
  write(1, "4", 1);
#endif

  XREAD(obj.fd, dyn, pht[nbr].p_filesz, NULL);
  for (nbr2 = 0; nbr2 < pht[nbr].p_filesz / sizeof(elfsh_Dyn); nbr2++)
    {
      if (dyn[nbr2].d_tag == DT_SYMTAB)
	obj.symoff = dyn[nbr2].d_un.d_val;
      else if (dyn[nbr2].d_tag == DT_STRTAB)
	obj.stroff = dyn[nbr2].d_un.d_val;
      else if (dyn[nbr2].d_tag == DT_STRSZ)
	obj.strsz = dyn[nbr2].d_un.d_val;
      else if (dyn[nbr2].d_tag == DT_PLTGOT)
	got = (elfsh_Addr) dyn[nbr2].d_un.d_val;
    }

#if __DEBUG_E2DBG__
  write(1, "5", 1);
#endif
 
  strtab = alloca(obj.strsz);
  if (!strtab)
    {
      write(1, " Failed to alloca string table\n", 31);
      return (-1);
    }
  if (!obj.symoff)
    {
     write(1, " Unable to find SYMOFF from PT_DYNAMIC\n", 39);
     return (-1);
    }

  if (!obj.stroff)
    {
      write(1, " Unable to find STROFF rom PT_DYNAMIC\n", 39);
      return (-1);
    }

  if (!obj.strsz)
    {
      write(1, " Unable to find STRSZ from PT_DYNAMIC\n", 39);
      return (-1);
    }
  
  if (!got)
    {
      write(1, " Unable to find PLTGOT from PT_DYNAMIC\n", 39);
      return (-1);
    }
 
#if __DEBUG_E2DBG__
  write(1, "6", 1);
#endif

  XSEEK(obj.fd, obj.stroff, SEEK_SET, NULL);
  XREAD(obj.fd, strtab, obj.strsz, NULL);

#if __DEBUG_E2DBG__
  write(1, "7", 1);
#endif

  /* XXX: Assume that strtab is always just after symtab */
  for (curoff = 0; obj.symoff + curoff < obj.stroff; curoff += sizeof(elfsh_Sym))
    {
      XSEEK(obj.fd, obj.symoff + curoff, SEEK_SET, NULL);
      XREAD(obj.fd, &cursym, sizeof(elfsh_Sym), NULL);
      if (cursym.st_name >= obj.strsz)
	continue;
      if (!strcmp(strtab + cursym.st_name, refsym))
	found_ref = cursym.st_value;
    }

#if __DEBUG_E2DBG__
  write(1, "8", 1);
#endif

  if (!found_ref)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to find reference symbol in object", NULL);

#if __DEBUG_E2DBG__
  write(1, " Success !\n", 11);
  len = snprintf(buf, sizeof(buf), 
		 " [*] REFADDR = %08X and FOUNDREF = %08X and GOT = %08X \n", 
		 refaddr, found_ref, got);
  write(1, buf, len);
#endif

  /* The reference addr is useful to deduce library base addresses */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     got + refaddr - found_ref);
}




  

/* Our own dlsym without malloc */
/* Useful in the early stage of mapping when malloc symbol is not yet known */
elfsh_Addr		e2dbg_dlsym(char *sym2resolve)
{
  e2dbgobj_t		obj;
  elfsh_Dyn		*dyn;
  u_int			nbr2;
  elfsh_Sym		cursym;
  char			*strtab;
  u_int			curoff;
  elfsh_Addr		found_sym = 0;
  elfshlinkmap_t	*curobj;

#if __DEBUG_E2DBG__
  char		buf[BUFSIZ];
  u_int		len;
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First go after the e2dbg and the binary linkmap entries */
  curobj = e2dbgworld.syms.map;
  while (curobj->lname == NULL || strstr(curobj->lname, "e2dbg") || *curobj->lname == 0x00)
    curobj = curobj->lnext;

  /* Iterate on the linkmap to resolve symbols in library priority order */
  for (; curobj; curobj = curobj->lnext)
    {

#if __DEBUG_E2DBG__
      write(1, " [*] e2dbg_dlsym called for resolving ", 38);
      write(1, sym2resolve, strlen(sym2resolve));
      write(1, " in ", 4);
      write(1, curobj->lname, strlen(curobj->lname));
      write(1, "\n", 1);
#endif
      
      memset(&obj, 0x00, sizeof(obj));
      XOPEN(obj.fd, curobj->lname, O_RDONLY, 0, NULL);
      dyn = curobj->lld;
      
      /* Getting needed address in the memory mapped PT_DYNAMIC segment */
      for (nbr2 = 0; dyn[nbr2].d_tag != DT_NULL; nbr2++)
	{
	  if (dyn[nbr2].d_tag == DT_SYMTAB)
#if defined(__FreeBSD__)
	    obj.symoff = curobj->laddr + dyn[nbr2].d_un.d_val;
#else
	    obj.symoff = dyn[nbr2].d_un.d_val;
#endif
	  else if (dyn[nbr2].d_tag == DT_STRTAB)
#if defined(__FreeBSD__)
	    obj.stroff = curobj->laddr + dyn[nbr2].d_un.d_val;
#else
	    obj.stroff = dyn[nbr2].d_un.d_val;
#endif
	  else if (dyn[nbr2].d_tag == DT_STRSZ)
	    obj.strsz = dyn[nbr2].d_un.d_val;
	}
      strtab = (char *) obj.stroff;

      /* Checking if everything is ok */
      if (!obj.symoff)
	{
	  write(1, " Unable to find SYMOFF from PT_DYNAMIC\n", 39);
	  return (-1);
	}
      
      if (!obj.stroff)
	{
	  write(1, " Unable to find STROFF rom PT_DYNAMIC\n", 39);
	  return (-1);
	}
      
      if (!obj.strsz)
	{
	  write(1, " Unable to find STRSZ from PT_DYNAMIC\n", 39);
	  return (-1);
	}
      
#if __DEBUG_E2DBG__
      len = snprintf(buf, sizeof(buf), " [*] SYMOFF = %u (%08X), STROFF = %u (%08X), STRSZ = %u \n",
		     obj.symoff, obj.symoff, obj.stroff, obj.stroff, obj.strsz);
      write(1, buf, len);
#endif

      XCLOSE(obj.fd, NULL);
      
      /* XXX: Assume that strtab is always just after symtab */
      for (curoff = 0; obj.symoff + curoff < obj.stroff; curoff += sizeof(elfsh_Sym))
	{
	  memcpy(&cursym, (void *) obj.symoff + curoff, sizeof(elfsh_Sym));
	  if (cursym.st_name >= obj.strsz)
	    continue;
	  if (!strcmp(strtab + cursym.st_name, sym2resolve) && cursym.st_value)
	    {
	      found_sym = cursym.st_value;
      
#if __DEBUG_E2DBG__
	      len = snprintf(buf, sizeof(buf), " [*] FOUNDSYM (%s) = %08X \n", 
			     strtab + cursym.st_name, found_sym);
	      write(1, buf, len);
#endif

	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, curobj->laddr + found_sym);
	    }
	}
    }

  /* We did not find the symbol in the linkmap ... */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Unable to find looked up symbol in object", NULL);
}



/* Get the address of the linkmap without any malloc, for early stage resolving */
elfshlinkmap_t*		e2dbg_linkmap_getaddr()
{
  elfsh_Addr		baseaddr;
  char			path[BUFSIZ];
  char			*home;
  elfsh_Addr		*got;
  elfshlinkmap_t	*lm;
#if defined(__FreeBSD__)
  Obj_Entry		*oe;
#endif

#if __DEBUG_E2DBG__
  char      buf[BUFSIZ];
  u_int     len;
#endif	

#if __DEBUG_E2DBG__
  len = sprintf(buf, " [*] e2dbg_linkmap_getaddr called\n");
  write(1, buf, len);
#endif

  home = getenv("HOME");
  snprintf(path, BUFSIZ, "%s/.e2dbg/e2dbg.so", home);
  baseaddr = e2dbg_dlsect(path, ".got.plt", (elfsh_Addr) &reference, "reference");

#if __DEBUG_E2DBG__
  len = sprintf(buf, " [*] Base address - 1st = %08X\n", baseaddr);
  write(1, buf, len);
#endif

  got = (elfsh_Addr *) baseaddr;

#if __DEBUG_E2DBG__
  len = sprintf(buf, " [*] GOT address = %08X\n", (elfsh_Addr) got);
  write(1, buf, len);
#endif

  /* BSD has an intermediate structure between GOT[1] and the linkmap entry */
#if defined(__FreeBSD__)
  oe = (Obj_Entry *) got[1];
  lm = (elfshlinkmap_t *) &oe->linkmap;
#else
  lm   = (elfshlinkmap_t *) got[1];
#endif

#if __DEBUG_E2DBG__
  len = sprintf(buf, " [*] Guessed Linkmap address = %08X \n--------------\n", (elfsh_Addr) lm);
  write(1, buf, len);
#endif

  return (lm);
}



/* Resolve malloc/realloc/free from standard libc */
int		e2dbg_dlsym_init()
{
  static int	done = 0;

#if __DEBUG_E2DBG__
  char		buf[BUFSIZ];
  u_int		len;
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  write(1, " [D] e2dbg_dlsym_init CALLED\n", 29);

  if (done)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
  
  write(1, " [D] e2dbg_dlsym_init EXECUTED\n", 31);

  /* Get the address of the linkmap without calling malloc */
  e2dbgworld.syms.map = e2dbg_linkmap_getaddr();

  /* Only use our own dlsym here, do not use the libc handler */
  e2dbgworld.syms.mallocsym = (elfsh_Addr) e2dbg_dlsym("malloc");
  if (!e2dbgworld.syms.mallocsym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig malloc not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc MALLOC() sym = %08X \n", e2dbgworld.syms.mallocsym);
  write(1, buf, len);
#endif

  e2dbgworld.syms.callocsym = (elfsh_Addr) e2dbg_dlsym("calloc");
  if (!e2dbgworld.syms.callocsym)
    {
      dlerror();
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Orig calloc not found", (-1));
    }

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc CALLOC() sym = %08X \n", e2dbgworld.syms.callocsym);
  write(1, buf, len);
#endif

  e2dbgworld.syms.reallocsym = (elfsh_Addr) e2dbg_dlsym("realloc");
  if (!e2dbgworld.syms.reallocsym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig realloc not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc REALLOC() sym = %08X \n", e2dbgworld.syms.reallocsym);
  write(1, buf, len);
#endif

  e2dbgworld.syms.freesym = (elfsh_Addr) e2dbg_dlsym("free");
  if (!e2dbgworld.syms.freesym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig free not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc FREE() sym = %08X \n", e2dbgworld.syms.freesym);
  write(1, buf, len);
#endif

  e2dbgworld.syms.vallocsym = (elfsh_Addr) e2dbg_dlsym("valloc");
  if (!e2dbgworld.syms.vallocsym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig valloc not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc VALLOC() sym = %08X \n", e2dbgworld.syms.vallocsym);
  write(1, buf, len);
#endif


  e2dbgworld.syms.memalignsym = (elfsh_Addr) e2dbg_dlsym("memalign");
  if (!e2dbgworld.syms.memalignsym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig memalign not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc MEMALIGN() sym = %08X \n", e2dbgworld.syms.memalignsym);
  write(1, buf, len);
#endif

  e2dbgworld.syms.memalignhooksym = (elfsh_Addr) e2dbg_dlsym("__memalign_hook");
  if (!e2dbgworld.syms.memalignhooksym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig __memalign_hook not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc __MEMALIGN_HOOK() sym = %08X \n", 
		 e2dbgworld.syms.memalignhooksym);
  write(1, buf, len);
#endif


  e2dbgworld.syms.mallochooksym = (elfsh_Addr) e2dbg_dlsym("__malloc_hook");
  if (!e2dbgworld.syms.mallochooksym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig __malloc_hook not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc __MALLOC_HOOK() sym = %08X \n", 
		 e2dbgworld.syms.mallochooksym);
  write(1, buf, len);
#endif


  /*
    e2dbgworld.syms.pthstartupsym = (elfsh_Addr) e2dbg_dlsym("__libc_malloc_pthread_startup");
    if (!e2dbgworld.syms.pthstartupsym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
    "Orig pthread_startup not found", (-1));
    #if __DEBUG_E2DBG__
    len = snprintf(buf, sizeof(buf), 
    " [*] Libc PTHREAD_STARTUP() sym = %08X \n", e2dbgworld.syms.pthstartupsym);
    write(1, buf, len);
    #endif
  */

  e2dbgworld.syms.pthreadcreate = (elfsh_Addr) e2dbg_dlsym("pthread_create");
  if (!e2dbgworld.syms.pthreadcreate)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig pthread_create not found", (-1));

  e2dbgworld.syms.pthreadexit   = (elfsh_Addr) e2dbg_dlsym("pthread_exit");
  if (!e2dbgworld.syms.pthreadexit)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig pthread_exit not found", (-1));

  e2dbgworld.syms.signal        = (elfsh_Addr) e2dbg_dlsym("signal");
  if (!e2dbgworld.syms.signal)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig signal not found", (-1));

  /* Now we can use malloc cause all symbols are resolved */
  done = 1;
  hash_init(&e2dbgworld.threads, 29);

  write(1, " [D] e2dbg_dlsym_init FINISHED\n", 31);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}

