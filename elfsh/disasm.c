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
    for (actual = world.curjob->list; actual != NULL; actual = actual->next)
      {  
	if (!actual->linkmap)
	  continue;

	name = elfsh_reverse_symbol(actual, addr, &offset);
	dname = elfsh_reverse_dynsymbol(actual, addr, &doffset);

	if (!name || (offset < 0) || (dname && doffset < offset && doffset >= 0))
	  {
	    name = dname;
	    offset = doffset;
	  }  

	if (!bestname || (bestoffset < 0) || (name && offset < bestoffset && offset >= 0))
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
void		do_resolve(void *data, u_int vaddr, char *buf, u_int maxlen)
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
u_int		display_instr(int fd, u_int index, u_int vaddr, u_int foffset, 
			      u_int size, char *name, u_int nindex, char *buff,
			      char *bigbuf, u_int *biglen)
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
  
  //#define	BIGBUFSZ (BUFSIZ * 64)

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
  ret = asm_read_instr(&ptr, buff + index, size - index + 10, 
		       world.curjob->proc);
  s = (!ret ? "(bad)" : 
       asm_display_instr_att(&ptr, (vaddr ? vaddr + index : 0)));

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
	  //*biglen += snprintf(bigbuf + *biglen, BIGBUFSZ - *biglen, "%s", logbuf);
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

	  //*biglen += snprintf(bigbuf + *biglen, BIGBUFSZ - *biglen, 
	  //	      "%s\n", logbuf);
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
	  vm_output(logbuf);
	  vm_output("\n");
	}
      vm_endline();


    }
  else
    write(fd, s, strlen(s));

#undef BIGBUFSZ

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}







/* Disassemble a function */
int             display_object(elfshobj_t *file, elfshsect_t *parent,
			       elfsh_Sym *sym, int size, u_int off,
			       u_int foffset, u_int vaddr, char *name, 
			       char otype)
{
  char		*buff;
  u_int		index;
  elfsh_SAddr   idx_bytes;
  char		buf[256];
  char		base[16] = "0123456789ABCDEF";
  elfsh_Addr    loff;
  char		str[256];
  //elfshsect_t	*sect;
  elfshsect_t	*targ;
  char		*s;
  //elfsh_SAddr	sct_index;
  u_int		ret;
  char		logbuf[BUFSIZ];

  char		bigbuf[BUFSIZ * 64];
  unsigned int	biglen = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Special case if the symbol is a plt entry */
  if (sym && elfsh_is_pltentry(file, sym) == 1 && size > ELFSH_PLT_ENTRY_SIZE)
    size = ELFSH_PLT_ENTRY_SIZE;

#if __DEBUG_DISASM__
  snprintf(logbuf, BUFSIZ - 1, 
	   "[debug:display_object] %s off(%u) size(%u) vaddr(%08X) foffset(%u), parent(%p, %s) \n",
	   name, off, size, vaddr, foffset, parent, (parent ? parent->name : "UNK"));
  vm_output(logbuf);
#endif

  /* Get the pointer on relevant data */
  buff = elfsh_get_raw(parent);
  index = off;
  buff += (vaddr - (parent->parent->rhdr.base + parent->shdr->sh_addr));
  
  /* Filter requests on void sections (ex: bss) */
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
	      vm_output("-- symbol size is bigger (use an offset) --\n");
	      break;
	    }
	  
	  /* For each entry of the array */
	  /* Dont forget the section offset at the end */
	  loff = *(elfsh_Addr *) (char*) elfsh_get_raw(parent) + 
	    index * sizeof(elfsh_Addr) + (vaddr - parent->shdr->sh_addr);
	  
	  //printf("second step passed \n");
	  
	  snprintf(buf, sizeof(buf), " %08X [foff: %u] \t %s[%0*u] = %08X", 
		   elfsh_is_debug_mode() ? 
		   file->rhdr.base + vaddr + index * sizeof(elfsh_Addr) :
		   vaddr   + index * sizeof(elfsh_Addr), 
		   foffset + index * sizeof(elfsh_Addr), 
		   name, 
		   ((sym->st_size / sizeof(elfsh_Addr)) < 100  ? 2 : 
		    (sym->st_size / sizeof(elfsh_Addr)) < 1000 ? 3 : 4),
		   index,
		   loff);
	  
	  //printf("third step passed, trying parent section on addr %08X \n", loff);
	  
	  /* If the target pointer is not valid */
	  targ = elfsh_get_parent_section(file, loff, &off);
	  if (targ == NULL || strcmp(targ->name, ELFSH_SECTION_NAME_RODATA))
	    {
	      //printf("Step 3A \n");
	      
	      s = elfsh_reverse_symbol(file, loff, &idx_bytes);
	      if (NULL == s || idx_bytes > 1000)
		s = elfsh_reverse_dynsymbol(file, loff, &idx_bytes);
	      
	      //printf("Step 3A2 (sect->name = %08X) \n", sect->name);
	      
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
	      
	      //printf("Step 3A3 (%s = %08X) \n", s);
	      
	      if (idx_bytes)
		snprintf(str, sizeof(str), "%s + %u", 
			 (s ? s : "<?>"), (u_int) idx_bytes);
	      else
		snprintf(str, sizeof(str), "<IRREVELANT VADDR>");
	      snprintf(logbuf, BUFSIZ, "%-75s %s \n", buf, str);
	      
	      //printf("Step 3A4 \n");
	    }
	  
	  /* else if yes, print the pointed data too */
	  else
	    {
	      //printf("Step 3B \n");
	      
	      s = elfsh_get_raw(targ);
	      s += off;
	      memcpy(str, s, 
		     (sizeof(str) > (targ->shdr->sh_size - off)) ?
		     targ->shdr->sh_size - off : sizeof(str));   
	      snprintf(logbuf, BUFSIZ - 1, "%-75s \"%s\" \n", buf, str);
	    }
	  
	  vm_output(logbuf);
	  
	  //printf("fourth step passed \n");
	}
    }
  
  
  
  /* We want asm + hexa output of the code */
  else if (otype == ELFSH_DISASM_VIEW)
    {
      if (elfsh_is_debug_mode())
	vaddr += file->rhdr.base;

      while (index < size)
	if (index >= 1000)
	  {
	    vm_output("-- symbol size is bigger (use an offset) --\n");
	    break;
	  }
	else
	  index += display_instr(-1, index, vaddr, foffset, size, name, 
				 index, buff, bigbuf, &biglen);
      
      // To uncomment one day
      // Second pattern in action
      //if ((world.curjob->curcmd->use_regx[1] == 0) || !regexec(&second->name, bigbuf, 0, 0, 0))
      //vm_output(bigbuf);

      if (world.curjob->curcmd->use_regx[1] == 0)
	vm_output("\n [*] No binary pattern was specified \n");

    }

  /* We want hexa + ascii output of the data */
  else if (ELFSH_HEXA_VIEW == otype)
    {

      if (name == NULL || !*name)
	name = ELFSH_NULL_STRING;

      while (index < size)
	{

	  /* Keep a limit to avoid output overflow */
	  if (index >= 1000)
	    {
	      vm_output("-- symbol size is bigger (use an offset) --\n");
	      break;
	    }
	  
	  /* Take care of quiet mode */
	  if (world.state.vm_quiet)
	    {
	      sprintf(buf, " %08X %s + %u", 
		      vaddr + index, name, index);
	      snprintf(logbuf, BUFSIZ - 1, "%-40s ", buf);
	      vm_output(logbuf);
	    }
	  else
	    {
	      sprintf(buf, " %08X [foff: %08u] %s + %u", 
		      vaddr + index, foffset + index, name, index);

	      //biglen += snprintf(bigbuf + biglen, sizeof(bigbuf) - biglen, "%s", logbuf);
	      //vm_output(logbuf);

	      snprintf(logbuf, BUFSIZ - 1, "%-60s ", buf);

	      //biglen += snprintf(bigbuf + biglen, sizeof(bigbuf) - biglen, "%s", logbuf);

	      vm_output(logbuf);
	    }

	  ret = (world.state.vm_quiet ? 8 : 16);

	  /* Print hexa */
	  for (loff = 0; loff < ret; loff++)
	    {
	      snprintf(logbuf, BUFSIZ - 1, "%c%c ", 
		       (index + loff < size ? 
			(base[(buff[index + loff] >> 4) & 0x0F]) : ' '),
		       (index + loff < size ? 
			(base[(buff[index + loff] >> 0) & 0x0F]) : ' '));
	      vm_output(logbuf);	     
	    }

	  /* Print ascii */
	  for (loff = 0; loff < ret; loff++)
	    {
	      char	c[2];

	      c[0] = buff[index + loff];
	      c[1] = 0x00;
	      vm_output(index + loff >= size ? " " :
			PRINTABLE(buff[index + loff]) ? 
			c : ".");
	    }

	  //biglen += snprintf(bigbuf + biglen, sizeof(bigbuf) - biglen, "\n");
	  vm_output("\n");
	  index += ret;

	}
    }

  //XFREE(buff);
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}






/* Print all variables and functions of the section */
int		display_section(elfshobj_t	*file, 
				elfshsect_t	*s, 
				char		*name, 
				u_int		off, 
				u_int		reqsize, 
				char		otype)
{
  elfsh_Sym	*actual;
  int		size;
  int		index;
  elfsh_SAddr  	offset;
  int		tot;
  char		*symname;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Hello message ;) */
  snprintf(logbuf, BUFSIZ - 1, " [*] Analysing section %s [*] \n\n", name);
  vm_output(logbuf);
  actual = elfsh_get_symtab(file, &size);
  tot = 0;
  if (s && !elfsh_get_raw(s))
    elfsh_get_anonymous_section(file, s);
  
  /* Display all symbols pointing in the section */
  if (actual)
    for (index = 0; index < size; index++)
      if (elfsh_get_parent_section(file, actual[index].st_value, &offset) == s)
	{
	  symname = elfsh_get_symbol_name(file, actual + index);
	  display_object(file, s, actual + index, 
			 (reqsize ? reqsize : actual[index].st_size), off,
			 s->shdr->sh_offset + actual[index].st_value - s->shdr->sh_addr,
			 actual[index].st_value, symname, otype);
	  
	  tot++;
	}

  /* If no symbol points to our section, we display it as a whole */
  if (!tot)
    {
      actual = elfsh_get_symbol_by_name(file, name);
      display_object(file, s, actual, (reqsize ? reqsize : s->shdr->sh_size), 
		     off, s->shdr->sh_offset, s->shdr->sh_addr, name, otype);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}










/* Display ASM code for a given function, or every functions of a section */
int             cmd_disasm()
{
  elfshlist_t	*actual;
  elfshsect_t	*s;
  elfshobj_t	*file;
  elfsh_Sym	*sym;
  char		*name;
  int		index;
  int		off;
  int		matchs;
  u_int		vaddr;
  char		logbuf[BUFSIZ];
  char		special = 0; /* foffset or vaddr form */

  unsigned int	saved_size;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
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
	  special = 1;
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
	  special = 1;
	}
    }
  
  /* If one of those special regex matched */
  if (special)
    {

#if __DEBUG_DISASM__
      snprintf(logbuf, BUFSIZ - 1, 
	       "[debug:cmd_disasm] SPECIAL with vaddr(%08X) \n", vaddr);
      vm_output(logbuf);
#endif
      
      if (file->hdr->e_type == ET_DYN && elfsh_is_debug_mode())
	vaddr -= file->rhdr.base;

      sym = elfsh_get_symbol_by_value(file, vaddr, &off,
				      ELFSH_LOWSYM);
      
      if (file->hdr->e_type == ET_DYN && elfsh_is_debug_mode())
	vaddr += file->rhdr.base;
      
      /*if (sym == NULL)
	sym = elfsh_get_symbol_by_value(file, vaddr, &off,
	ELFSH_HIGHSYM);*/

      if (sym == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No matching symbol for offset", -1);

      actual->off += off;
      if (!actual->size)
	actual->size = elfsh_get_symbol_size(sym);
      actual->size += actual->off;
      name = elfsh_get_symbol_name(file, sym);
      matchs++;
      
#if __DEBUG_DISASM__
      snprintf(logbuf, BUFSIZ - 1, 
	       "[debug_disasm:cmd_disasm] Found special regx (%s with off %d)  sym = %08X\n", 
	       name, off, sym->st_value);
      vm_output(logbuf);
#endif

      s = elfsh_get_parent_section(file, vaddr, NULL);
      if (!s)
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "No matching section for address", -1);

      display_object(file, 
		     s, sym, actual->size, actual->off,
		     elfsh_get_foffset_from_vaddr(file, vaddr),
		     vaddr, name, actual->otype);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  
  /* Now find matches in the section header table (SHT) */
  for (s = file->sectlist; s != NULL; s = s->next)
    {
      name = s->name;
      if (name == NULL || !*name || regexec(&actual->name, name, 0, 0, 0))
	{

#if __DEBUG_DISASM__
          snprintf(logbuf, BUFSIZ - 1,
                   "[debug_disasm:cmd_disasm] Passing on-disk section %u (%s) with name = %s (shdr = %08X addr = %08X) \n", 
		   s->index, s->name, actual->rname, s->shdr, s->shdr->sh_addr);
          vm_output(logbuf);
#endif
	  
	  continue;
	}
      matchs++;
      
#if __DEBUG_DISASM__
     snprintf(logbuf, BUFSIZ - 1, 
	      "[debug_disasm:cmd_disasm] Found section regx (%s) \n", name);
     vm_output(logbuf);
#endif
      
      display_section(file, s, name, actual->off, 
		      actual->size + actual->off, 
		      actual->otype);
    }



  /* Now find matches in the runtime section header table (RSHT) */
  for (s = file->rsectlist; s != NULL; s = s->next)
    {
      name = s->name;
      if (name == NULL || !*name || regexec(&actual->name, name, 0, 0, 0))
	{

#if __DEBUG_DISASM__
          snprintf(logbuf, BUFSIZ - 1,
                   "[debug_disasm:cmd_disasm] Passing runtime section %u (%s) with name = *%s* (shdr = %08X, addr = %08X) \n", 
		   s->index, name, actual->rname, s->shdr, s->shdr->sh_addr);
          vm_output(logbuf);
#endif

	  continue;
	}
      matchs++;
      
#if __DEBUG_DISASM__
     snprintf(logbuf, BUFSIZ - 1, 
	      "[debug_disasm:cmd_disasm] Found runtime section regx (%s) \n", 
	      name);
     vm_output(logbuf);
#endif
      
      display_section(file, s, name, actual->off, 
		      actual->size + actual->off, 
		      actual->otype);
    }

  
  saved_size = actual->size;

  /* Now find matches in the symbol table .symtab */
  if (file->secthash[ELFSH_SECTION_SYMTAB] != NULL)
    for (sym = file->secthash[ELFSH_SECTION_SYMTAB]->altdata, index = 0; 
	 index < file->secthash[ELFSH_SECTION_SYMTAB]->shdr->sh_size / sizeof(elfsh_Sym); 
	 index++)
      {
	name = elfsh_get_symbol_name(file, sym + index);
	if (name != NULL && *name && DUMPABLE(sym + index) && 
	    elfsh_get_symbol_type(sym + index) != STT_SECTION &&
	    !regexec(&actual->name, name, 0, 0, 0))
	  {
	    matchs++;
	    if (!actual->size)
	      actual->size = elfsh_get_symbol_size(sym + index);
	    actual->size += actual->off;

#if __DEBUG_DISASM__
	    snprintf(logbuf, BUFSIZ - 1, 
		     "[debug_disasm:cmd_disasm] Found symbol regx (%s) as=(%i)\n", 
		     name, actual->size);
	    vm_output(logbuf);
#endif

	    s = elfsh_get_parent_section(file, sym[index].st_value, NULL);
	    if (!s)
	      {
		//printf("Passed SYM match without section %s [%08X] \n", name, sym[index].st_value);
		continue;
	      }

	    display_object(file, s, sym + index, actual->size, actual->off,
			   elfsh_get_foffset_from_vaddr(file, sym[index].st_value),
			   sym[index].st_value, name, actual->otype);
	    actual->size = saved_size;
	  }
      }
  

  /* Now find matches in the dynamic symbol table .dynsym */
  if (file->secthash[ELFSH_SECTION_DYNSYM] != NULL)
    for (sym = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSYM]), index = 0; 
	 index < file->secthash[ELFSH_SECTION_DYNSYM]->shdr->sh_size / sizeof(elfsh_Sym); 
	 index++)
      {

	name = elfsh_get_dynsymbol_name(file, sym + index);

	if (name != NULL && *name && DUMPABLE(sym + index) &&
	    !regexec(&actual->name, name, 0, 0, 0))
	  {
	    matchs++;
	    if (!actual->size)
	      actual->size = elfsh_get_symbol_size(sym + index);
	    actual->size += actual->off;

#if 1//__DEBUG_DISASM__
	   snprintf(logbuf, BUFSIZ - 1, 
		    "[debug_disasm:cmd_disasm] Found dynsym regx (%s) \n", name);
	   vm_output(logbuf);
#endif

	   printf("st_value : %x\n", elfsh_get_symbol_value(sym + index));
	   if (elfsh_get_symbol_value(sym + index) == 0)
	     {
		 elfsh_toggle_mode();
	       sym = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSYM]);
	       elfsh_toggle_mode();
	       printf("toggled st_value with base : %x\n", 
		      elfsh_get_symbol_value(sym + index));
	       s = elfsh_get_parent_section(file, 
					    file->rhdr.base + sym[index].st_value, 
					    NULL);
	     }
	   else
	     s = elfsh_get_parent_section(file, sym[index].st_value, NULL);

	   if (!s)
	     {
	       printf("Passed DYNSYM match without section %s [%08X] \n", 
		      name, sym[index].st_value);
	       continue;
	     }

	    display_object(file, s, sym + index, 
			   actual->size, actual->off,
			   elfsh_get_foffset_from_vaddr(file, sym[index].st_value),
			   sym[index].st_value, name, actual->otype);
	  }
      }
  
  /* Printing a warning message if we have no match */
  if (!world.state.vm_quiet && !matchs)
    {
      snprintf(logbuf, BUFSIZ - 1, " [E] No match for request %s\n\n", 
	       actual->rname);
      vm_output(logbuf);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



