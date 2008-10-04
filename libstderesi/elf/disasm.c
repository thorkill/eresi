/**
** @file disasm.c
** 
** Started on  Fri Nov  2 15:41:34 2001 jfv
**
**
** $Id: disasm.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"


static revmlist_t* second = NULL;


/**
 * Resolve symbol in one file or all (mapped) if we are in e2dbg
 * Runtime compatible.
 * @param file
 * @param addr
 * @param roffset
 * @return
*/
char		*revm_resolve(elfshobj_t *file, eresi_Addr addr, 
			      elfsh_SAddr *roffset)
{
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
  char		**keys;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", NULL);

  actual = file;
  name = elfsh_reverse_symbol(actual, addr, &offset);
  dname = elfsh_reverse_dynsymbol(actual, addr, &doffset);

#if __DEBUG_RESOLVE__
  printf("[elfsh:resolve] First found file : %s name = %s:%d / dname = %s:%d ("XFMT") \n", 
	 actual->name, name, offset, dname, doffset, addr);
#endif

  if (!name || (dname && !strcmp(name, ELFSH_SECTION_NAME_PLT)) || 
      (offset < 0) || (dname && doffset < offset && doffset >= 0))
    {
      name = dname;
      offset = doffset;
    }  
  else if (name && dname && doffset == offset)
    name = dname;

  bestname = name;
  bestoffset = offset;
  bestfile = actual;
  
  /* Find the best symbol by searching in all the objects of the process */
  if (e2dbg_presence_get())
    {
      keys = hash_get_keys(&world.curjob->loaded, &keynbr);
      for (index = 0; index < keynbr; index++)
	{
	  actual = hash_get(&world.curjob->loaded, keys[index]);
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
	      (bestoffset < 0) || (name && (offset < bestoffset) && offset >= 0))
	    {
	      bestname = name;
	      bestoffset = offset;
	      bestfile = actual;

#if __DEBUG_RESOLVE__
	      fprintf(stderr, "[elfsh:resolve] Changed best : file %s name %s %d\n", 
		  actual->name, name, offset);
#endif

	    }
	  
	}
    }

#if __DEBUG_RESOLVE__
  printf("[elfsh:resolve] BEST name %s %d\n", bestname, bestoffset);
#endif

  if (roffset)
    *roffset = bestoffset;
  
  if (bestname == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to resolve best name", (NULL));

  if (elfsh_is_debug_mode())
    {
      str = revm_basename(bestfile->name);
      snprintf(buf, BUFSIZ, "%s@%s", 
	       bestname, (str ? str : "CORRUPTED"));
    }
  else
    snprintf(buf, BUFSIZ, "%s", bestname);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, strdup(buf));
 
}


/**
 *  Symbol resolving handler for libasm.
 * Runtime compatible 
 * @param data
 * @param vaddr
 * @param buf
 * @param maxlen
 */
void		asm_do_resolve(void *data, eresi_Addr vaddr, 
			       char *buf, unsigned int maxlen)
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
  if (NULL != parent && parent->shdr->sh_addr)
    name = revm_resolve(file, vaddr, &off);      
  else
    name = NULL;

  /* Print the symbol name in 'buf' so that libasm can print it */
  len = (NULL == name ? 10 : strlen(name) + 25);
  if (name != NULL && *name != 0x00)
    {
      sep = (off > 0 ? " + " : off < 0 ? " - " : "");
      len = snprintf(buf, maxlen - 1, "%s%s%s%s", 
		     revm_colorfieldstr("<"),
		     revm_colortypestr(name), 
		     (off ? revm_colorfieldstr(sep) : ""),
		     (off ? "" : revm_colorfieldstr(">")));

      if (off)
	snprintf(buf + len, maxlen - len -  1, "%s%s", 
		 revm_colornumber("%u", (unsigned int) off),
		 revm_colorfieldstr(">"));
    }
  else
    snprintf(buf, maxlen - 1, AFMT, vaddr);
}



/*------------------ to clean !! ******/




/** 
 * Display An instruction.
 * Runtime compatible 
 * @param fd
 * @param index
 * @param vaddr
 * @param foffset
 * @param size
 * @param name
 * @param nindex
 * @param buff
 */
unsigned int		revm_instr_display(int fd, unsigned int index, eresi_Addr vaddr, 
				 unsigned int foffset, unsigned int size, char *name, 
				 unsigned int nindex, char *buff)
			      
{
  char		*s;
  char		buf[256];
  unsigned int		idx_bytes;
  unsigned int		ret;
  asm_instr	ptr;
  char		base[16] = "0123456789ABCDEF";
  char		logbuf[BUFSIZ];
  char		c1[2];
  char		c2[2];
  unsigned int		strsz;
  elfsh_Half	machine;
  unsigned int		delta;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!buff)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", (-1));    

  /* Init proc */			  
  if (!world.curjob->proc) 
    {
      switch (machine = elfsh_get_arch(world.curjob->curfile->hdr))
	{
	case EM_386:
	  world.curjob->proc = &world.proc;
	  delta = 10;
	  break;
	case EM_SPARC:
	case EM_SPARC32PLUS:
	case EM_SPARCV9:
	  world.curjob->proc = &world.proc_sparc;
	  delta = 0;
	  break;
	case EM_MIPS:
	case EM_MIPS_RS3_LE:
	case EM_MIPS_X:
	  world.curjob->proc = &world.proc_mips;
	  delta = 0;
	  break;
	default:
	  snprintf(logbuf, sizeof (logbuf) - 1, 
		   "Architecture %s not supported. No disassembly available\n",
		   elfsh_get_machine_string(machine));
	  revm_output(logbuf);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
    }

  /* Print the instr. itself : vaddr and relative symbol resolution */
  ret = asm_read_instr(&ptr, (unsigned char *)buff + index, size - index + delta, 
		       world.curjob->proc);
  if (ret == -1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "asm_read_instruction faild (-1)", (ret));

  s = (!ret ? "(bad)" : asm_display_instr_att(&ptr, (vaddr ? vaddr + index : 0)));

  /* Libasm test */
  if (fd == -1)
    {

      /* Are we in quiet mode ? */
      if (world.state.revm_quiet)
	{
	  snprintf(buf, sizeof(buf), " %s %s + %s", 
		   revm_coloraddress(XFMT, vaddr + index), 
		   revm_colorstr(name),
		   revm_colornumber("%u", nindex));
	  size = snprintf(logbuf, BUFSIZ, "%-40s %-30s ", 
			  buf, revm_colorinstr(s));
	}
      else
	{
	  size = snprintf(buf, sizeof(buf), " %s [%s %s] %s + %s", 
			  revm_coloraddress(XFMT, vaddr + index), 
			  revm_colorfieldstr("foff:"), 
			  revm_colornumber("%u", foffset + index), 
			  revm_colorstr(name), 
			  revm_colornumber("%u", nindex));
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
			  revm_colorinstr(s));
	}
      
      /* Print bytes in hexa for this instruction */
      ret = asm_instr_len(&ptr);
      if (!ret)
	ret++;

      if (!world.state.revm_quiet)
	for (idx_bytes = 0; idx_bytes < ret; idx_bytes++)
	  {
	    c1[0] = base[(buff[index + idx_bytes] >> 4) & 0x0F];
	    c2[0] = base[buff[index + idx_bytes] & 0x0F];
	    c1[1] = c2[1] = 0x00;
	    size += snprintf(logbuf + size, sizeof(logbuf) - size, "%s%s ", 
			     revm_colorfieldstr(c1), 
			     revm_colorfieldstr(c2));
	  }

      if (!world.curjob->curcmd || !world.curjob->curcmd->use_regx[1] || 
	  !regexec(&second->name, logbuf, 0, 0, 0))
	{
	  revm_output(logbuf);
	  revm_output("\n");
	}
      revm_endline();
    }
  else
    write(fd, s, strlen(s));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}




/** 
 * @brief Display the content of an objet in hexadecimal and ascii format
 * @param parent The parent section for the object
 * @param name The name of the object
 * @param vaddr The virtual address where the object stands
 * @param index The offset where to start display
 * @param size  Number of bytes to display
 * @param off  The buffer offset be skipped at the beginning (duplicate!)
 * @param sym The symbol associated to this object
 * @param buff The data pointer of the object
 * @param foffset The file offset where the object data stands at.
 * @return Always 0.
 */
int		revm_hexa_display(elfshsect_t *parent, char *name, eresi_Addr vaddr, unsigned int index, 
				  unsigned int size, unsigned int off, char *buff, unsigned int foffset)
{
  eresi_Addr	vaddr2;
  unsigned int		curidx;
  char		buf[256];
  char		logbuf[BUFSIZ];
  char		tmp[BUFSIZ];
  char		c1[2], c2[2];
  unsigned int		ret;
  unsigned int		loff;  
  char		*pStr;
  char		base[16] = "0123456789ABCDEF";

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);      
   if (name == NULL || !*name)
     name = ELFSH_NULL_STRING;
   vaddr2 = vaddr;
   curidx = 0;

   
   while (index < size && size > 0)
     {
       
       /* Take care of quiet mode */
       if (world.state.revm_quiet)
	 {
	   sprintf(buf, " %s %s + %s", 
		   revm_coloraddress(AFMT, (eresi_Addr) vaddr2), 
		   revm_colorstr(name), 
		   revm_colornumber("%u", (parent ? index : index + off)));
	   snprintf(logbuf, BUFSIZ - 1, "%-40s ", buf);
	   revm_output(logbuf);
	 }
       else
	 {
	   sprintf(buf, " %s [%s %s] %s + %s", 
		   revm_coloraddress(AFMT, (eresi_Addr) vaddr2), 
		   revm_colorfieldstr("foff:"),
		   revm_colornumber(DFMT, foffset + curidx), 
		   revm_colorstr(name), 
		   revm_colornumber("%u", (parent ? index : index + off)));
	   snprintf(logbuf, BUFSIZ - 1, "%-*s", 60 + revm_color_size(buf), buf);
	   revm_output(logbuf);
	 }
       revm_endline();
       ret = (world.state.revm_quiet ? 8 : 16);
       tmp[0] = c1[1] = c2[1] = 0x00;
       
       /* Print hexa */
       for (loff = 0; loff < ret; loff++)
	 {
	   c1[0] = c2[0] = ' ';
	      if (index + loff < size)
		{
		  c1[0] = base[(buff[curidx + loff] >> 4) & 0x0F];
		  c2[0] = base[(buff[curidx + loff] >> 0) & 0x0F];
		}
	      snprintf(logbuf, BUFSIZ - 1, "%s%s ", c1, c2);
	      if (strlen(tmp) + strlen(logbuf) < BUFSIZ)
		strcat(tmp, logbuf);
	 }
       
       revm_output(revm_colorfieldstr(tmp));
       revm_endline();
       tmp[0] = 0x00;
       
       /* Print ascii */
       for (loff = 0; loff < ret; loff++)
	 {
	      c1[0] = buff[curidx + loff];
	      pStr = (index + loff >= size ? " " : 
		      (PRINTABLE(buff[curidx + loff]) ? c1 : "."));
	      if (strlen(tmp) + 1 < BUFSIZ)
		strcat(tmp, pStr);
	 }
       
       revm_output(revm_colorstr(tmp));
       revm_endline();
       revm_output("\n");
       index += ret;
       vaddr2 += ret;
       curidx += ret;
     }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}











/** 
 * @brief Display the content of an array object
 * @param parent The parent section for the array object
 * @param sym The symbol associated to this array
 * @param buff The data pointer of the array
 * @param vaddr The virtual address where the array stands
 * @param name The name of the array
 * @param foffset The file offset of the array data in ELF
 * @return Always 0.
 */
int		revm_array_display(elfshsect_t *parent, elfsh_Sym *sym, char *buff, eresi_Addr vaddr,
				   char *name, unsigned int foffset)
{
  char		buf[256];
  char		str[256];
  char		logbuf[BUFSIZ];
  char		*tmpbuff;
  eresi_Addr	vaddr2;
  eresi_Addr	loff;
  elfshsect_t	*targ;
  unsigned int		index;
  unsigned int		off;
  char		*s;
  elfsh_SAddr	idx_bytes;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);      
  for (index = 0; index * sizeof(eresi_Addr) < sym->st_size; index++)
    {
      
      /* Do not print more than 250 entries at a time */
      /* Use an offset for more dumping */
      if (index >= 250)
	{
	  revm_output("-- symbol size is bigger (use an offset) --\n");
	  break;
	}
      
      /* For each entry of the array */
      /* Dont forget the section offset at the end */
      if (!parent)
	tmpbuff = buff;
      else
	{
	  tmpbuff  = elfsh_get_raw(parent);
	  tmpbuff += vaddr - (parent->parent->rhdr.base + parent->shdr->sh_addr);
	}
      
#if defined(KERNSH)
      if (kernsh_is_mem_mode())
	parent->parent->rhdr.base = 0;
#endif
      
      tmpbuff += index * sizeof(eresi_Addr);
      loff     = * (eresi_Addr *) tmpbuff;
      vaddr2   = vaddr + index * sizeof(eresi_Addr);
      
      snprintf(buf, sizeof(buf), " " AFMT " [foff: %u] \t %s[%0*u] = " XFMT, 
	       (elfsh_is_debug_mode() && parent ? parent->parent->rhdr.base + vaddr2 : vaddr2),
	       foffset + index * sizeof(eresi_Addr), name,
	       ((sym->st_size / sizeof(eresi_Addr)) < 100  ? 2 : 
		(sym->st_size / sizeof(eresi_Addr)) < 1000 ? 3 : 4),
	       index, loff);
      
      /* If the object was given as address, dont try to resolve array entries 
	 -- FIXME: we could try to find if the entries are valid (mapped) on any object */
      if (!parent)
	{
	  revm_output(buf);
	  continue;
	}
      
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
		     (s ? s : "<?>"), (unsigned int) idx_bytes);
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
      revm_output(logbuf);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}




/** 
 * @brief Display the content of a binary object (wrapper function)
 * @param parent
 * @param sym
 * @param size
 * @param off
 * @param foffset
 * @param vaddr
 * @param name
 * @param otype
 * @return
 */
int             revm_object_display(elfshsect_t *parent, elfsh_Sym *sym, int size, 
				  unsigned int off, unsigned int foffset, eresi_Addr vaddr, 
				  char *name, char otype)
{
  char		*buff;
  unsigned int		index;
  elfsh_SAddr   idx_bytes;
  int		value;
  int		nbrinstr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!parent && !elfsh_is_debug_mode())
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "parent section is NULL", -1);

  /* Special case if the symbol is a plt entry */
  if (parent && sym && elfsh_is_pltentry(parent->parent, sym) == 1 && 
      size > ELFSH_PLT_ENTRY_SIZE)
    size = ELFSH_PLT_ENTRY_SIZE;

#if __DEBUG_DISASM__
  snprintf(logbuf, BUFSIZ - 1, 
	   "[debug:revm_object_display] %s off(%u) size(%u) vaddr(%08X) "
	   "foffset(%u), parent(%p, %s) \n",
	   name, off, size, vaddr, foffset, parent, 
	   (parent ? parent->name : "UNK"));
  revm_output(logbuf);
#endif

  /* Get the pointer on relevant data */
  if (parent)
    {
      buff  = elfsh_get_raw(parent);
      buff += (vaddr - (parent->parent->rhdr.base + parent->shdr->sh_addr));
      index = off;
    }
  else
    {
      buff = (void *) vaddr;
      index = 0;
    }

#if defined(KERNSH)  
  if (kernsh_is_mem_mode())
      parent->parent->rhdr.base = 0;
#endif  

  /* Filter requests on void sections (ex: bss when not inserted in file) */
  if (!elfsh_is_debug_mode() && (!parent || !parent->data))
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"No data at this address", -1);
  
  /* If the regex match a pointers array, print the pointed elements too */
  if (sym && 
      (elfsh_get_symbol_type(sym) == STT_OBJECT  ||
       elfsh_get_symbol_type(sym) == STT_COMMON) && 
      !(sym->st_size % sizeof(eresi_Addr)))
    {
      revm_array_display(parent, sym, buff, vaddr, name, foffset);
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* We want assembly + hexa, or only hexa + ascii ? */  
  switch (otype)
    {

      /* We want assembly and hexa */
    case REVM_VIEW_DISASM:
#if defined(KERNSH)
      if (!kernsh_is_present() && elfsh_is_debug_mode())
	vaddr += parent->parent->rhdr.base;
#else
      if (elfsh_is_debug_mode() && parent)
	vaddr += parent->parent->rhdr.base;
#endif
      
      idx_bytes = (sym && sym->st_value ? vaddr + index - sym->st_value : index);
      for (nbrinstr = 0; nbrinstr < size && size > 0; nbrinstr++)
	{
	  value = revm_instr_display(-1, index, vaddr, 
				   foffset, size, name,
				   idx_bytes, buff);
	  if (value <= 0)
	    break;
	  index += value;
	  idx_bytes += value;
	}
      break;

  /* We want hexa + ascii output of the data */
    case REVM_VIEW_HEX:
      revm_hexa_display(parent, name, vaddr, index, size, off, buff, foffset);
    }
  
  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}






/** 
 * Print all variables and functions of the section.
 * @param s
 * @param name
 * @param re
 * @return
 */
int		revm_section_display(elfshsect_t	*s, 
				     char		*name, 
				     revmlist_t		*re)
{
  elfsh_Sym	*actual;
  int		size, symtab_size;
  int		index;
  elfsh_SAddr  	offset;
  int		tot;
  char		*symname;
  char		logbuf[BUFSIZ];
  int		err;
  eresi_Addr	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Hello message ;) */
  snprintf(logbuf, BUFSIZ - 1, " [*] Analysing section %s [*] \n\n", name);
  revm_output(logbuf);
  actual = elfsh_get_symtab(s->parent, &symtab_size);
  tot = 0;
  if (s && !elfsh_get_raw(s))
    elfsh_get_anonymous_section(s->parent, s);
  
  if (!actual)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
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
	err     = revm_object_display(s, actual + index, size, re->off, addr,
				    actual[index].st_value, symname, re->otype);
	
	if (err == -1)
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, err);
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
      if (revm_object_display(s, actual, size, re->off, s->shdr->sh_offset, 
			    s->shdr->sh_addr, name, re->otype) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to display section data", -1);
    }

  /* Everything went ok */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/** 
 * Match regular expressions in a SHT (SHT or RSHT).
 * @param file
 * @param l
 * @param actual
 * @return
 */
int		revm_match_sht(elfshobj_t *file, elfshsect_t *l, revmlist_t *actual)
{
  elfshsect_t	*s;
  char		*name;
  int		matchs = 0;
#if __DEBUG_DISASM__
  char		logbuf[BUFSIZ];
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
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
     revm_output(logbuf);
#endif
      
     if (revm_section_display(s, name, actual) < 0)
       PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, matchs);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, matchs);
}





/** 
 * Match regular expressions in the symbol tables 
 * @param file
 * @param symtab
 * @param actual
 * @param flag
 * @return
 */
int		revm_match_symtab(elfshobj_t *file, elfshsect_t *symtab, 
				revmlist_t *actual, int flag)
{
  elfshsect_t	*s;
  char		*name;
  int		matchs = 0;
  unsigned int		saved_size;
  elfsh_Sym	*sym;
  eresi_Addr	addr;
  int		index;
#if __DEBUG_DISASM__
  char		logbuf[BUFSIZ];
#endif

  /* Natural checks */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!file || !symtab || !actual)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
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
	       "[debug_disasm:cmd_disasm] Found dynsym regx (%s) (" AFMT ")\n", 
	       name, elfsh_get_symbol_value(sym + index));
      revm_output(logbuf);
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
      if (revm_object_display(s, sym + index, actual->size, actual->off, addr,
			    sym[index].st_value, name, actual->otype) == -1)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Error while displaying matched object", -1);
      
      if (!flag)
	actual->size = saved_size;
    }

  /* Everything went ok */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, matchs);
}






/** 
 * Match a special regular expression.
 * @param file
 * @param vaddr
 * @param actual
 * @return
 */
int		revm_match_special(elfshobj_t *file, eresi_Addr vaddr,
				   revmlist_t *actual)
{
  elfsh_Sym	*sym;
  char		*name;
  int		off;
  unsigned int		matchs;
  elfshsect_t	*s;

#if __DEBUG_DISASM__
  char		logbuf[BUFSIZ];
#endif
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
#if __DEBUG_DISASM__
  snprintf(logbuf, BUFSIZ - 1, 
	   "[debug:cmd_disasm] SPECIAL with vaddr(" AFMT ") \n", vaddr);
  revm_output(logbuf);
#endif
  
  if (file->hdr->e_type == ET_DYN && elfsh_is_debug_mode())
    vaddr -= file->rhdr.base;
  sym = elfsh_get_symbol_by_value(file, vaddr, &off, ELFSH_LOWSYM);
  if (file->hdr->e_type == ET_DYN && elfsh_is_debug_mode())
    vaddr += file->rhdr.base;
  if (sym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
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
	   "(%s with off %d)  sym = " AFMT "\n", name, off, sym->st_value);
  revm_output(logbuf);
#endif

  s = elfsh_get_parent_section(file, vaddr, NULL);
  if (!s && !elfsh_is_debug_mode())
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No matching section for address", -1);

  if (!s)
    {
      actual->size = sizeof(unsigned long);
      //actual->off = 0;
    }
  else if (!actual->size)
    actual->size = s->shdr->sh_size;

  revm_object_display(s, sym, actual->size, actual->off,
		      elfsh_get_foffset_from_vaddr(file, vaddr),
		      vaddr, name, actual->otype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Display ASM code for a given function, or every functions of a section 
 */
int             cmd_disasm()
{
  revmlist_t	*actual;
  elfshobj_t	*file;
  revmexpr_t	*expr;
  int		matchs;
  eresi_Addr	vaddr;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Make sure we get symtabs of current object */
  elfsh_get_symtab(world.curjob->curfile, NULL);
  elfsh_get_dynsymtab(world.curjob->curfile, NULL);
  revm_output("\n");
  
  /* now walk the regex list for this option */
  actual = world.curjob->curcmd->disasm + 0;
  second = world.curjob->curcmd->disasm + 1;
  matchs = vaddr = 0;
  file = world.curjob->curfile;

  /* If the regex contains a vaddr instead of a symbol name */
  if (actual->rname)
    {
      if (*actual->rname == REVM_VAR_PREFIX)
	{
	  expr = revm_expr_get(actual->rname);
	  if (!expr || !expr->value || !expr->value->immed_val.ent)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Invalid requested address expression", -1);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     revm_match_special(file, expr->value->immed_val.ent, actual));
	}
      else if (IS_VADDR(actual->rname))
	{
	  if (sscanf(actual->rname + 2, AFMT, &vaddr) != 1)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Invalid virtual address requested", -1);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			revm_match_special(file, vaddr, actual));
	}
  
      /* else if it contains a file offset */
      else if (isdigit((int) *actual->rname))
	{
	  vaddr = elfsh_get_vaddr_from_foffset(file, atoi(actual->rname));
	  if (vaddr == 0xFFFFFFFF && 
	      sscanf(actual->rname + 2, AFMT, &vaddr) != 1)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Invalid file offset requested",
			      -1);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     revm_match_special(file, vaddr, actual));
	}
    }

  /* Try to match in the sectlist and runtime sectlist */
  matchs += revm_match_sht(file, file->sectlist, actual);
  matchs += revm_match_sht(file, file->rsectlist, actual);
  
  /* Now find matches in the symbol table .symtab */
  /* Last parameter says if we are in symtab or dynsymtab */
  matchs += revm_match_symtab(file, file->secthash[ELFSH_SECTION_SYMTAB], actual, 0);
  matchs += revm_match_symtab(file, file->secthash[ELFSH_SECTION_DYNSYM], actual, 1);
  
  /* Printing a warning message if we have no match */
  if (!world.state.revm_quiet && !matchs)
    {
      snprintf(logbuf, BUFSIZ - 1, " [E] No match for request %s\n\n", 
	       actual->rname);
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



