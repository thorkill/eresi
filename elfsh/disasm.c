/*
** disasm.c for elfsh
** 
** Started on  Fri Nov  2 15:41:34 2001 mayhem
**
*/
#include "elfsh.h"


static elfshlist_t* second = NULL;



/* Resolve symbol in one file or all (mapped) if we are in e2dbg */
/* Runtime compatible */
char		*vm_resolve(elfshobj_t *file, elfsh_Addr addr, 
			    elfsh_SAddr *roffset)
{
  hashent_t	*ent;
  int		index;
  elfshobj_t	*actual;
  char		*name = NULL;
  char		*dname = NULL;
  elfsh_SAddr	offset = 0;
  elfsh_SAddr	doffset = 0;
  char		*bestname = NULL;
  elfsh_SAddr	bestoffset;
  elfshobj_t	*bestfile;
  char		buf[BUFSIZ];
  char		*str;


  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", NULL);

  actual = file;
  name = elfsh_reverse_symbol(actual, addr, &offset);
  dname = elfsh_reverse_dynsymbol(actual, addr, &doffset);

  if (!name || (dname && !strcmp(name, ELFSH_SECTION_NAME_PLT)) || 
      (offset < 0) || (dname && doffset < offset && doffset >= 0))
    {
      name = dname;
      offset = doffset;
    }  
  
  bestname = name;
  bestoffset = offset;
  bestfile = actual;
  

#if __DEBUG_RESOLVE__
  printf("[elfsh:resolve] file : %s name %s %d\n", actual->name, name, offset);
#endif
  
  /* Find the best symbol by searching in all the objects of the process */
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
      for (index = 0; index < world.curjob->loaded.size; index++)
	for (ent = &world.curjob->loaded.ent[index];
	     ent != NULL && ent->key != NULL;
	     ent = ent->next)
	  {
	    actual = ent->data;
	    if (!actual->linkmap)
	      continue;
	    
	    name = elfsh_reverse_symbol(actual, addr, &offset);
	    dname = elfsh_reverse_dynsymbol(actual, addr, &doffset);
	    
	    if (!name || (offset < 0) || 
		(dname && doffset < offset && doffset >= 0))
	      {
		name = dname;
		offset = doffset;
	      }  
	    
	    if (!bestname || 
		(bestoffset < 0) || (name && offset < bestoffset && offset >= 0))
	      {
		bestname = name;
		bestoffset = offset;
		bestfile = actual;
	      }
	    
#if __DEBUG_RESOLVE__
	    printf("[elfsh:resolve] file : %s name %s %d\n", 
		   actual->name, name, offset);
#endif
	  }
  
#if __DEBUG_RESOLVE__
  printf("[elfsh:resolve] BEST name %s %d\n", bestname, bestoffset);
#endif

  if (roffset)
    *roffset = bestoffset;
  
  if (bestname == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to resolve best name", (NULL));

  if (elfsh_is_debug_mode())
    {
      str = vm_basename(bestfile->name);
      snprintf(buf, BUFSIZ, "%s@%s", 
	       bestname, (str ? str : "CORRUPTED"));
    }
  else
    snprintf(buf, BUFSIZ, "%s", bestname);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, elfsh_strdup(buf));
}


/* Symbol resolving handler for libasm */
/* Runtime compatible */
void		asm_do_resolve(void *data, elfsh_Addr vaddr, 
			       char *buf, u_int maxlen)
{
  elfshobj_t	*file;
  elfshsect_t	*parent;
  char		*name;
  elfsh_SAddr	off;
  int		len;
  char		*sep;

  /* Retreive the nearest symbol */
  file = data;
  parent = elfsh_get_parent_section(file, vaddr, 0);
  if (NULL != parent && NULL != parent->shdr->sh_addr)
    name = vm_resolve(file, vaddr, &off);      
  else
    name = NULL;

  /* Print the symbol name in 'buf' so that libasm can print it */
  len = (NULL == name ? 10 : strlen(name) + 25);
  if (name != NULL && *name != 0x00)
    {
      sep = (off > 0 ? " + " : off < 0 ? " - " : "");
      len = snprintf(buf, maxlen - 1, "%s%s%s%s", 
		     vm_colorfieldstr("<"),
		     vm_colortypestr(name), 
		     (off ? vm_colorfieldstr(sep) : ""),
		     (off ? "" : vm_colorfieldstr(">")));

      if (off)
	snprintf(buf + len, maxlen - len -  1, "%s%s", 
		 vm_colornumber("%u", (u_int) off),
		 vm_colorfieldstr(">"));

      //printf("[%u] %s\n", strlen(buf), buf);

    }
  else
    snprintf(buf, maxlen - 1, "%X", vaddr);
}



/*------------------ to clean !! ******/




/* Display An instruction */
/* Runtime compatible */
u_int		vm_display_instr(int fd, u_int index, elfsh_Addr vaddr, 
				 u_int foffset, u_int size, char *name, 
				 u_int nindex, char *buff)
			      
{
  char		*s;
  char		buf[256];
  u_int		idx_bytes;
  u_int		ret;
  asm_instr	ptr;
  char		base[16] = "0123456789ABCDEF";
  char		logbuf[BUFSIZ];
  char		c1[2];
  char		c2[2];
  u_int		strsz;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!buff)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", (-1));    

  /* Init proc */			  
  if (!world.curjob->proc) {
  
    switch (elfsh_get_arch(world.curjob->current->hdr))
    {
    case EM_386:
      world.curjob->proc = &world.proc;
      break;
    case EM_SPARC:
      world.curjob->proc = &world.proc_sparc;
      break;
    default:
      vm_output("Architecture not supported. No disassembly available\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
   }

  /* Print the instr. itself : vaddr and relative symbol resolution */
  ret = asm_read_instr(&ptr, (u_char *)buff + index, size - index + 10, 
		       world.curjob->proc);
  s = (!ret ? "(bad)" : asm_display_instr_att(&ptr, (vaddr ? vaddr + index : 0)));

  /* Libasm test */
  if (fd == -1)
    {

      /* Are we in quiet mode ? */
      if (world.state.vm_quiet)
	{
	  snprintf(buf, sizeof(buf), " %s %s + %s", 
		   vm_coloraddress(XFMT, vaddr + index), 
		   vm_colorstr(name),
		   vm_colornumber("%u", nindex));
	  size = snprintf(logbuf, BUFSIZ, "%-40s %-30s ", 
			  buf, vm_colorinstr(s));
	}
      else
	{
	  size = snprintf(buf, sizeof(buf), " %s [%s %s] %s + %s", 
			  vm_coloraddress(XFMT, vaddr + index), 
			  vm_colorfieldstr("foff:"), 
			  vm_colornumber("%u", foffset + index), 
			  vm_colorstr(name), 
			  vm_colornumber("%u", nindex));
	  strsz = strlen(s);
	  size = snprintf(logbuf, BUFSIZ, "%-*s %-*s ", 
			  (size > 95 ? 125 : 
			   size > 87 ? 100 : 
			   size > 75 ? 108 : 
			   size > 50 ? 88 : 55),
			  buf,
			  (strsz > 95 ? 125 : 
			   strsz > 87 ? 100 : 
			   strsz > 75 ? 108 : 
			   strsz > 50 ? 88 : 55),
			  vm_colorinstr(s));
	}
      
      /* Print bytes in hexa for this instruction */
      ret = asm_instr_len(&ptr);
      if (!ret)
	ret++;

      if (!world.state.vm_quiet)
	for (idx_bytes = 0; idx_bytes < ret; idx_bytes++)
	  {
	    c1[0] = base[(buff[index + idx_bytes] >> 4) & 0x0F];
	    c2[0] = base[buff[index + idx_bytes] & 0x0F];
	    c1[1] = c2[1] = 0x00;
	    size += snprintf(logbuf + size, sizeof(logbuf) - size, "%s%s ", 
			     vm_colorfieldstr(c1), 
			     vm_colorfieldstr(c2));
	  }

      if ((world.curjob->curcmd->use_regx[1] == 0) || 
	  !regexec(&second->name, logbuf, 0, 0, 0))
	{
	  VM_OUTPUT_Q(logbuf);
	  VM_OUTPUT_Q("\n");
	}
      vm_endline();
    }
  else
    write(fd, s, strlen(s));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}







/* Disassemble a function */
int             vm_display_object(elfshsect_t *parent, elfsh_Sym *sym, int size, 
				  u_int off, u_int foffset, elfsh_Addr vaddr, 
				  char *name, char otype)
{
  char		*buff;
  u_int		index;
  elfsh_SAddr   idx_bytes;
  char		buf[256];
  char		base[16] = "0123456789ABCDEF";
  elfsh_Addr    loff;
  char		str[256];
  elfshsect_t	*targ;
  char		*s;
  u_int		ret;
  int		value;
  char		logbuf[BUFSIZ];
  char		tmp[BUFSIZ];
  char		c1[2], c2[2];
  char		*pStr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Special case if the symbol is a plt entry */
  if (sym && elfsh_is_pltentry(parent->parent, sym) == 1 && 
      size > ELFSH_PLT_ENTRY_SIZE)
    size = ELFSH_PLT_ENTRY_SIZE;

#if __DEBUG_DISASM__
  snprintf(logbuf, BUFSIZ - 1, 
	   "[debug:vm_display_object] %s off(%u) size(%u) vaddr(%08X) "
	   "foffset(%u), parent(%p, %s) \n",
	   name, off, size, vaddr, foffset, parent, 
	   (parent ? parent->name : "UNK"));
  vm_output(logbuf);
#endif

  /* Get the pointer on relevant data */
  buff  = elfsh_get_raw(parent);
  index = off;
  buff += (vaddr - (parent->parent->rhdr.base + parent->shdr->sh_addr));
  
  /* Filter requests on void sections (ex: bss when not inserted in file) */
  if (!parent || !parent->data)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			"No data at this address", -1);
  
  /* If the regex match a pointers array, print the pointed elements too */
  if (sym && 
      (elfsh_get_symbol_type(sym) == STT_OBJECT  ||
       elfsh_get_symbol_type(sym) == STT_COMMON) && 
      !(sym->st_size % sizeof(elfsh_Addr)))
    {
      
      for (index = 0; index * sizeof(elfsh_Addr) < sym->st_size; index++)
	{
	  
	  /* Do not print more than 250 entries at a time */
	  /* Use an offset for more dumping */
	  if (index >= 250)
	    {
	      VM_OUTPUT("-- symbol size is bigger (use an offset) --\n");
	      break;
	    }
	  
	  /* For each entry of the array */
	  /* Dont forget the section offset at the end */
	  loff = *(elfsh_Addr *) (char*) elfsh_get_raw(parent) + 
	    index * sizeof(elfsh_Addr) + (vaddr - parent->shdr->sh_addr);
	  
	  snprintf(buf, sizeof(buf), " %08X [foff: %u] \t %s[%0*u] = %08X", 
		   elfsh_is_debug_mode() ? 
		   parent->parent->rhdr.base + vaddr + index * sizeof(elfsh_Addr) :
		   vaddr   + index * sizeof(elfsh_Addr), 
		   foffset + index * sizeof(elfsh_Addr), 
		   name, 
		   ((sym->st_size / sizeof(elfsh_Addr)) < 100  ? 2 : 
		    (sym->st_size / sizeof(elfsh_Addr)) < 1000 ? 3 : 4),
		   index,
		   loff);

	  /* If the target pointer is not valid */
	  targ = elfsh_get_parent_section(parent->parent, loff, 
					  (elfsh_SAddr *) &off);
	  if (targ == NULL || strcmp(targ->name, ELFSH_SECTION_NAME_RODATA))
	    {
	      s = elfsh_reverse_symbol(parent->parent, loff, &idx_bytes);
	      if (NULL == s || idx_bytes > 1000)
		s = elfsh_reverse_dynsymbol(parent->parent, loff, &idx_bytes);
	      if (NULL == s || idx_bytes > 1000)
		{
		  if (targ != NULL)
		    {
		      s = targ->name;
		      idx_bytes = off;
		    }
		  else
		    idx_bytes = 0;
		}
	      if (idx_bytes)
		snprintf(str, sizeof(str), "%s + %u", 
			 (s ? s : "<?>"), (u_int) idx_bytes);
	      else
		snprintf(str, sizeof(str), "<IRREVELANT VADDR>");
	      snprintf(logbuf, BUFSIZ, "%-75s %s \n", buf, str);
	    }

	  /* else if yes, print the pointed data too */
	  else
	    {
	      s = elfsh_get_raw(targ);
	      s += off;
	      memcpy(str, s, 
		     (sizeof(str) > (targ->shdr->sh_size - off)) ?
		     targ->shdr->sh_size - off : sizeof(str));   
	      snprintf(logbuf, BUFSIZ - 1, "%-75s \"%s\" \n", buf, str);
	    }
	  
	  /* maybe the user asked to quit the display */
	  VM_OUTPUT(logbuf);
	}
    }
  
  
  
  /* We want asm + hexa output of the code */
  else if (otype == ELFSH_DISASM_VIEW)
    {
      if (elfsh_is_debug_mode())
	vaddr += parent->parent->rhdr.base;

      while (index < size && size > 0)
	{
	  value = vm_display_instr(-1, index, vaddr, foffset, size, name,
				   index, buff);

	  if (value <= 0)
	    break;
	  
	  index += value;
	}
    }

  /* We want hexa + ascii output of the data */
  else if (ELFSH_HEXA_VIEW == otype)
    {
      if (name == NULL || !*name)
	name = ELFSH_NULL_STRING;

      while (index < size && size > 0)
	{
	  /* Take care of quiet mode */
	  if (world.state.vm_quiet)
	    {
	      sprintf(buf, " %s %s + %s", 
		      vm_coloraddress("%08x", (elfsh_Addr) vaddr + index), 
		      vm_colorstr(name), vm_colornumber("%u", index));
	      snprintf(logbuf, BUFSIZ - 1, "%-40s ", buf);
	      vm_output(logbuf);
	    }
	  else
	    {
	      sprintf(buf, " %s [%s %s] %s + %s", 
		      vm_coloraddress("%08x", (elfsh_Addr) vaddr + index), 
		      vm_colorfieldstr("foff:"),
		      vm_colornumber("%08u", foffset + index), 
		      vm_colorstr(name), vm_colornumber("%u", index));
	      snprintf(logbuf, BUFSIZ - 1, "%-*s", 60 + vm_color_size(buf), buf);
	      vm_output(logbuf);
	    }
	  vm_endline();
	  ret = (world.state.vm_quiet ? 8 : 16);
	  tmp[0] = c1[1] = c2[1] = 0x00;

	  /* Print hexa */
	  for (loff = 0; loff < ret; loff++)
	    {
	      c1[0] = c2[0] = ' ';
	      if (index + loff < size)
		{
		  c1[0] = base[(buff[index + loff] >> 4) & 0x0F];
		  c2[0] = base[(buff[index + loff] >> 0) & 0x0F];
		}
	      snprintf(logbuf, BUFSIZ - 1, "%s%s ", c1, c2);
	      if (strlen(tmp) + strlen(logbuf) < BUFSIZ)
		strcat(tmp, logbuf);
	    }

	  vm_output(vm_colorfieldstr(tmp));
	  vm_endline();
	  tmp[0] = 0x00;

	  /* Print ascii */
	  for (loff = 0; loff < ret; loff++)
	    {
	      c1[0] = buff[index + loff];
	      pStr = (index + loff >= size ? " " : 
		      (PRINTABLE(buff[index + loff]) ? c1 : "."));
	      if (strlen(tmp) + 1 < BUFSIZ)
		strcat(tmp, pStr);
	    }

	  vm_output(vm_colorstr(tmp));
	  vm_endline();
	  vm_output("\n");
	  index += ret;
	}
  }

  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}






/* Print all variables and functions of the section */
int		vm_display_section(elfshsect_t	*s, 
				   char		*name, 
				   elfshlist_t	*re)
{
  elfsh_Sym	*actual;
  int		size, symtab_size;
  int		index;
  elfsh_SAddr  	offset;
  int		tot;
  char		*symname;
  char		logbuf[BUFSIZ];
  int		err;
  elfsh_Addr	addr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Hello message ;) */
  snprintf(logbuf, BUFSIZ - 1, " [*] Analysing section %s [*] \n\n", name);
  vm_output(logbuf);
  actual = elfsh_get_symtab(s->parent, &symtab_size);
  tot = 0;
  if (s && !elfsh_get_raw(s))
    elfsh_get_anonymous_section(s->parent, s);
  
  if (!actual)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section has no symbol associated", -1);

  /* Display all symbols data pointing in the section */
  for (index = 0; index < symtab_size; index++)
    if (elfsh_get_parent_section(s->parent, actual[index].st_value, &offset) == s)
      {
	if (re->size)
	  size = ((re->size + re->off) > actual[index].st_size ? 
		  actual[index].st_size : re->size + re->off);
	else
	  size = actual[index].st_size;
	
	symname = elfsh_get_symbol_name(s->parent, actual + index);
	addr    = s->shdr->sh_offset + actual[index].st_value - s->shdr->sh_addr;
	err     = vm_display_object(s, actual + index, size, re->off, addr,
				    actual[index].st_value, symname, re->otype);
	
	if (err == -1)
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, err);
	tot++;
      }

  /* If no symbol points to our section, we display it as a whole */
  if (!tot)
    {
      if (re->size)
	size = ((re->size + re->off) > s->shdr->sh_size ? s->shdr->sh_size :
		re->size + re->off);
      else
	size = s->shdr->sh_size;
      actual = elfsh_get_symbol_by_name(s->parent, name);
      if (vm_display_object(s, actual, size, re->off, s->shdr->sh_offset, 
			    s->shdr->sh_addr, name, re->otype) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to display section data", -1);
    }

  /* Everything went ok */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Match regular expressions in a SHT (SHT or RSHT) */
int		vm_match_sht(elfshobj_t *file, elfshsect_t *l, elfshlist_t *actual)
{
  elfshsect_t	*s;
  char		*name;
  int		matchs = 0;
#if __DEBUG_DISASM__
  char		logbuf[BUFSIZ];
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Now find matches in the list of section */
  for (s = l; s != NULL; s = s->next)
    {
      name = s->name;
      if (name == NULL || !*name || regexec(&actual->name, name, 0, 0, 0))
	continue;
      matchs++;
      
#if __DEBUG_DISASM__
     snprintf(logbuf, BUFSIZ - 1, 
	      "[debug_disasm:cmd_disasm] Found section regx (%s) \n", name);
     vm_output(logbuf);
#endif
      
     if (vm_display_section(s, name, actual) < 0)
       ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, matchs);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, matchs);
}





/* Match regular expressions in the symbol tables */
int		vm_match_symtab(elfshobj_t *file, elfshsect_t *symtab, 
				elfshlist_t *actual, int flag)
{
  elfshsect_t	*s;
  char		*name;
  int		matchs = 0;
  u_int		saved_size;
  elfsh_Sym	*sym;
  elfsh_Addr	addr;
  int		index;
#if __DEBUG_DISASM__
  char		logbuf[BUFSIZ];
#endif

  /* Natural checks */
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!file || !symtab || !actual)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  saved_size = actual->size;
  
  /* Iterate on symbols */
  sym = (flag ? elfsh_get_raw(symtab) : (elfsh_Sym *) symtab->altdata);
  for (index = 0; index < symtab->shdr->sh_size / sizeof(elfsh_Sym); index++)
    {
      
      /* Match ? */
      name = (flag ? elfsh_get_dynsymbol_name(file, sym + index) : 
	      elfsh_get_symbol_name(file, sym + index));      
      if (name == NULL || *name == 0x00 || !DUMPABLE(sym + index) ||
	  elfsh_get_symbol_type(sym + index) == STT_SECTION ||
	  regexec(&actual->name, name, 0, 0, 0))
	continue;
      matchs++;
      if (!actual->size)
	actual->size = elfsh_get_symbol_size(sym + index);
      else
	actual->size = ((actual->size + actual->off) > 
			elfsh_get_symbol_size(sym + index) ?
			elfsh_get_symbol_size(sym + index) : 
			actual->size + actual->off);
      
#if __DEBUG_DISASM__
      snprintf(logbuf, BUFSIZ - 1, 
	       "[debug_disasm:cmd_disasm] Found dynsym regx (%s) (%08X)\n", 
	       name, elfsh_get_symbol_value(sym + index));
      vm_output(logbuf);
#endif

      /* Only use toggle mode when inspecting .dynsym */
      if (flag && !elfsh_get_symbol_value(sym + index))
	{
	  elfsh_toggle_mode();
	  sym = elfsh_get_raw(symtab);
	  elfsh_toggle_mode();
	  s = elfsh_get_parent_section(file, 
				       file->rhdr.base + sym[index].st_value,
				       NULL);
	}
      else
	s = elfsh_get_parent_section(file, sym[index].st_value, NULL);

      /* Display matched object */
      addr = elfsh_get_foffset_from_vaddr(file, sym[index].st_value);
      if (vm_display_object(s, sym + index, actual->size, actual->off, addr,
			    sym[index].st_value, name, actual->otype) == -1)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Error while displaying matched object", -1);
      
      if (!flag)
	actual->size = saved_size;
    }

  /* Everything went ok */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, matchs);
}






/* Match a special regular expression */
int		vm_match_special(elfshobj_t *file, elfsh_Addr vaddr,
				 elfshlist_t *actual)
{
  elfsh_Sym	*sym;
  char		*name;
  int		off;
  u_int		matchs;
  elfshsect_t	*s;

#if __DEBUG_DISASM__
  char		logbuf[BUFSIZ];
#endif
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
#if __DEBUG_DISASM__
  snprintf(logbuf, BUFSIZ - 1, 
	   "[debug:cmd_disasm] SPECIAL with vaddr(%08X) \n", vaddr);
  vm_output(logbuf);
#endif
  
  if (file->hdr->e_type == ET_DYN && elfsh_is_debug_mode())
    vaddr -= file->rhdr.base;
  sym = elfsh_get_symbol_by_value(file, vaddr, &off, ELFSH_LOWSYM);
  if (file->hdr->e_type == ET_DYN && elfsh_is_debug_mode())
    vaddr += file->rhdr.base;
  if (sym == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No matching symbol for offset", -1);
  actual->off += off;
  if (!actual->size)
    actual->size = elfsh_get_symbol_size(sym);
  else
    actual->size = ((actual->size + actual->off) > elfsh_get_symbol_size(sym) ?
		    elfsh_get_symbol_size(sym) : actual->size + actual->off);
  name = elfsh_get_symbol_name(file, sym);
  matchs++;
  
#if __DEBUG_DISASM__
  snprintf(logbuf, BUFSIZ - 1, "[debug_disasm:cmd_disasm] Found special regx "
	   "(%s with off %d)  sym = %08X\n", name, off, sym->st_value);
  vm_output(logbuf);
#endif

  s = elfsh_get_parent_section(file, vaddr, NULL);
  if (!s)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No matching section for address", -1);

  if (!actual->size)
    actual->size = s->shdr->sh_size;
  vm_display_object(s, sym, actual->size, actual->off,
		    elfsh_get_foffset_from_vaddr(file, vaddr),
		    vaddr, name, actual->otype);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}








/* Display ASM code for a given function, or every functions of a section */
int             cmd_disasm()
{
  elfshlist_t	*actual;
  elfshobj_t	*file;
  int		matchs;
  elfsh_Addr	vaddr;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First check the architecture */
  switch (elfsh_get_arch(world.curjob->current->hdr))
    {
    case EM_386:
      world.curjob->proc = &world.proc;
      break;
    case EM_SPARC:
      world.curjob->proc = &world.proc_sparc;
      break;
    default:
      vm_output("Architecture not supported. No disassembly available\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Make sure we get symtabs of current object */
  elfsh_get_symtab(world.curjob->current, NULL);
  elfsh_get_dynsymtab(world.curjob->current, NULL);
  vm_output("\n");
  
  /* now walk the regex list for this option */
  actual = world.curjob->curcmd->disasm + 0;
  second = world.curjob->curcmd->disasm + 1;
  matchs = vaddr = 0;
  file = world.curjob->current;

  /* If the regex contains a vaddr instead of a symbol name */
  if (actual->rname)
    {
      if (IS_VADDR(actual->rname))
	{
	  if (sscanf(actual->rname + 2, "%X", &vaddr) != 1)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Invalid virtual address requested", 
			      -1);
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     vm_match_special(file, vaddr, actual));
	}
  
      /* else if it contains a file offset */
      else if (isdigit((int) *actual->rname))
	{
	  vaddr = elfsh_get_vaddr_from_foffset(file, atoi(actual->rname));
	  if (vaddr == 0xFFFFFFFF && 
	      sscanf(actual->rname + 2, "%X", &vaddr) != 1)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Invalid file offset requested",
			      -1);
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     vm_match_special(file, vaddr, actual));
	}
    }

  /* Try to match in the sectlist and runtime sectlist */
  matchs += vm_match_sht(file, file->sectlist, actual);
  matchs += vm_match_sht(file, file->rsectlist, actual);
  
  /* Now find matches in the symbol table .symtab */
  /* Last parameter says if we are in symtab or dynsymtab */
  matchs += vm_match_symtab(file, file->secthash[ELFSH_SECTION_SYMTAB], actual, 0);
  matchs += vm_match_symtab(file, file->secthash[ELFSH_SECTION_DYNSYM], actual, 1);
  
  /* Printing a warning message if we have no match */
  if (!world.state.vm_quiet && !matchs)
    {
      snprintf(logbuf, BUFSIZ - 1, " [E] No match for request %s\n\n", 
	       actual->rname);
      vm_output(logbuf);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



