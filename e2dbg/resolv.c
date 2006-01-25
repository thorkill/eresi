/*
** resolv.c for e2dbg
**
** Contains the linkmap related resolving builtins in e2dbg
**
** Last Update Thu Oct 13 19:37:26 2005 mm
*/
#include "elfsh.h"


extern int		(*main)(int argc, char **argv);


/* Load linkmap in PIE based process */
static int		e2dbg_load_linkmap_pie(char *name)
{
  elfshobj_t		*handle;

  /*
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
  

/* Our own dlsym without malloc */
/* Useful in the early stage of mapping when malloc symbol is not yet known */
elfsh_Addr		e2dbg_dlsym(char *objname, char *sym2resolve, 
				    elfsh_Addr refaddr, char *refsym)
{
  e2dbgobj_t		obj;
  elfsh_Phdr		*pht;
  elfsh_Dyn		*dyn;
  u_int			nbr, nbr2;
  elfsh_Sym		cursym;
  char			*strtab;
  u_int			curoff;
  elfsh_Addr		found_ref = 0;
  elfsh_Addr		found_sym = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /*
  write(1, "e2dbg_dlsym called for resolving ", 34);
  write(1, sym2resolve, strlen(sym2resolve));
  write(1, " in ", 4);
  write(1, objname, strlen(objname));
  write(1, "\n", 1);
  */

  memset(&obj, 0x00, sizeof(obj));
  XOPEN(obj.fd, objname, O_RDONLY, 0, NULL);
  XREAD(obj.fd, &obj.e, sizeof(elfsh_Ehdr), NULL);

  //write(1, "1", 1);

  XSEEK(obj.fd, obj.e.e_phoff, SEEK_SET, NULL);
  pht = alloca(obj.e.e_phnum * sizeof(elfsh_Phdr));

  //write(1, "2", 1);

  XREAD(obj.fd, pht, obj.e.e_phnum * sizeof(elfsh_Phdr), NULL);
  for (nbr = 0; nbr < obj.e.e_phnum; nbr++)
    if (pht[nbr].p_type == PT_DYNAMIC)
      {
	obj.dynoff = pht[nbr].p_offset;
	break;
      }

  //write(1, "3", 1);

  XSEEK(obj.fd, obj.dynoff, SEEK_SET, NULL);
  dyn = alloca(pht[nbr].p_filesz);

  //write(1, "4", 1);

  XREAD(obj.fd, dyn, pht[nbr].p_filesz, NULL);
  for (nbr2 = 0; nbr2 < pht[nbr].p_filesz / sizeof(elfsh_Dyn); nbr2++)
    {
      //write(1, "|", 1);
      if (dyn[nbr2].d_tag == DT_SYMTAB)
	obj.symoff = dyn[nbr2].d_un.d_val;
      else if (dyn[nbr2].d_tag == DT_STRTAB)
	obj.stroff = dyn[nbr2].d_un.d_val;
      else if (dyn[nbr2].d_tag == DT_STRSZ)
	obj.strsz = dyn[nbr2].d_un.d_val;
    }

  //write(1, "5", 1);

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
  
  //write(1, "6", 1);

  XSEEK(obj.fd, obj.stroff, SEEK_SET, NULL);
  XREAD(obj.fd, strtab, obj.strsz, NULL);

  //write(1, "7", 1);

  /* XXX: Assume that strtab is always just after symtab */
  for (curoff = 0; obj.symoff + curoff < obj.stroff; curoff += sizeof(elfsh_Sym))
    {
      XSEEK(obj.fd, obj.symoff + curoff, SEEK_SET, NULL);
      XREAD(obj.fd, &cursym, sizeof(elfsh_Sym), NULL);
      if (cursym.st_name >= obj.strsz)
	continue;
      if (!strcmp(strtab + cursym.st_name, refsym))
	found_ref = cursym.st_value;
      else if (!strcmp(strtab + cursym.st_name, sym2resolve))
	found_sym = cursym.st_value;
    }

  //write(1, "8", 1);

  if (!found_sym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to find looked up symbol in object", NULL);
  if (!found_ref)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to find reference symbol in object", NULL);

  //write(1, " Success !\n", 11);

  /* The reference addr is useful to deduce library base addresses */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     found_sym + (refaddr - found_ref));
}


/* Resolve malloc/realloc/free from standard libc */
int		e2dbg_dlsym_init()
{
  static int	done = 0;
  char		*name;
  elfsh_Addr	symref;
  char		*refstr;

  char		buf[BUFSIZ];
  u_int		len;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  write(1, "DLSYM INIT CALLED\n", 18);

  if (done)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));

  write(1, "DLSYM INIT BACKED\n", 18);

  /* We use this as a reference symbol */
  symref = (elfsh_Addr) read;
  refstr = "read";

#if defined(linux)
  name = "/lib/libc.so.6";
#elif (defined(sun) && defined(__i386))
  name = "/lib/libc.so.6";
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  name = "/usr/lib/libc.so";
#else
  #error "No libc default path specified"
#endif

  /* Only use our own dlsym here, do not use the libc handler */
  e2dbgworld.mallocsym = (elfsh_Addr) e2dbg_dlsym(name, "malloc", 
						  symref, refstr);
  if (!e2dbgworld.mallocsym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig malloc not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc MALLOC() sym = %08X \n", e2dbgworld.mallocsym);
  write(1, buf, len);
#endif

  e2dbgworld.callocsym = (elfsh_Addr) e2dbg_dlsym(name, "calloc",
						  symref, refstr);
  if (!e2dbgworld.callocsym)
    {
      dlerror();
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Orig calloc not found", (-1));
    }

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc CALLOC() sym = %08X \n", e2dbgworld.callocsym);
  write(1, buf, len);
#endif


  e2dbgworld.reallocsym = (elfsh_Addr) e2dbg_dlsym(name, "realloc", 
						   symref, refstr);
  if (!e2dbgworld.reallocsym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig realloc not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc REALLOC() sym = %08X \n", e2dbgworld.reallocsym);
  write(1, buf, len);
#endif

  e2dbgworld.freesym = (elfsh_Addr) e2dbg_dlsym(name, "free", 
						symref, refstr);
  if (!e2dbgworld.freesym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig free not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc FREE() sym = %08X \n", e2dbgworld.freesym);
  write(1, buf, len);
#endif

  //e2dbg_dlclose(handle);

  done = 1;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


