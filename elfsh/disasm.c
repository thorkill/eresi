/*
** disasm.c for elfsh
** 
** Started on  Fri Nov  2 15:41:34 2001 mayhem
** Last update Wed Jun 25 08:53:57 2003 mayhem
*/
#include "elfsh.h"




#if defined(USE_LIBASM)
/* Symbol resolving handler for libasm */
void		do_resolve(void *data, u_int vaddr, char *buf, u_int maxlen)
{
  elfshobj_t	*file;
  elfshsect_t	*parent;
  char		*name;
  int		off;
  int		len;

#if __DEBUG_DISASM__
  printf("[DEBUG_DISASM:do_resolve] Resolving 0x%08X ... \n", vaddr);
#endif

  /* Retreive the nearest symbol */
  file = data;
  parent = elfsh_get_parent_section(file, vaddr, 0);
  if (NULL != parent && NULL != parent->shdr->sh_addr)
    {
      name = elfsh_reverse_metasym(file, vaddr, &off);      
#if __DEBUG_DISASM__
      printf("[DEBUG_DISASM:do_resolve] Found parent section %s \n", parent->name);
#endif
    }
  else
    name = NULL;

#if __DEBUG_DISASM__
  printf("[DEBUG_DISASM:do_resolve] Resolved *%s* \n", name);
#endif

  /* Print the symbol name in 'buf' so that libasm can print it */
  len = (NULL == name ? 10 : strlen(name) + 25);
  if (name != NULL && *name != 0x00)
    {
      len = snprintf(buf, maxlen - 1, "<%s%s%s", name, 
		     (off > 0 ? " + " : off < 0 ? " - " : ""),
		     (off ? "" : ">"));
      if (off)
	snprintf(buf + len, maxlen - len -  1, "%u>", off);
    }
  else
    snprintf(buf, maxlen - 1, "%X", vaddr);
}








/* Display an instruction */
u_int		display_instr(int fd, u_int index, u_int vaddr, u_int foffset, 
			      u_int size, char *name, u_int nindex, char *buff)
{
  char		*s;
  char		buf[256];
  u_int		idx_bytes;
  u_int		ret;
  asm_instr	ptr;
  char		base[16] = "0123456789ABCDEF";
 

  /* Print the instr. itself : vaddr and relative symbol resolution */
  ret = asm_read_instr(&ptr, buff + index, size - index + 10, &world.proc);
  s = (!ret ? "(bad)" : 
       asm_display_instr_att(&ptr, (vaddr ? vaddr + index : 0)));
  
  /* Are we in quiet mode ? */
  if (world.state.vm_quiet)
    {
      snprintf(buf, sizeof(buf), " %08X %s + %u", vaddr + index, name, nindex);
      dprintf(fd, "%-40s %-30s ", buf, s);
    }
  else
    {
      snprintf(buf, sizeof(buf), " %08X [foff: %u] %s + %u", 
	      vaddr + index, foffset + index, name, nindex);
      dprintf(fd, "%-60s %-50s ", buf, s);
    }
  
  /* Print bytes in hexa for this instruction */
  ret = asm_instr_len(&ptr);
  if (!ret)
    ret++;
  
  if (!world.state.vm_quiet)
    for (idx_bytes = 0; idx_bytes < ret; idx_bytes++)
      dprintf(fd, "%c%c ", 
	     base[(buff[index + idx_bytes] >> 4) & 0x0F], 
	     base[buff[index + idx_bytes] & 0x0F]);
  dprintf(fd, "\n");
  return (ret);
}
#endif	/* USE_LIBASM */








/* Disassemble a function */
int             display_object(elfshobj_t *file, Elf32_Sym *sym, int size,
				 u_int off, u_int foffset, u_int vaddr, 
				 char *name, char otype)
{
  char		*buff;
  u_int		index;
  u_int		idx_bytes;
  char		buf[256];
  char		base[16] = "0123456789ABCDEF";
  u_int		loff;
  char		str[256];
  elfshsect_t	*sect;
  char		*s;
  u_int		sct_index;
  u_int		ret;

  /* Special case if the symbol is a plt entry */
  if (sym && elfsh_is_pltentry(file, sym) == 1 && size > ELFSH_PLT_ENTRY_SIZE)
    size = ELFSH_PLT_ENTRY_SIZE;
  XALLOC(buff, size + 16, -1);

#if __DEBUG_DISASM__
  printf("[debug:display_object] %s off(%u) size(%u) vaddr(%08X) foffset(%u)\n", 
	 name, off, size, vaddr, foffset);
#endif

  /* Read raw data in the file */
  if (elfsh_raw_read(file, foffset, buff, size + 16) < 0)
    {
      elfsh_error();
      free(buff);
      printf(" [!] Cant read data for block %s (vaddr %08X, foff %u, size %u) \n\n", 
	     name, vaddr, foffset, size + 16);
      return (-1);
    }

  index = off;

  /* If the regex match a pointers array, print the pointed elements too */
  if (sym && 
      (elfsh_get_symbol_type(sym) == STT_OBJECT  ||
       elfsh_get_symbol_type(sym) == STT_COMMON) && 
      !(sym->st_size % sizeof(long)))
    {

      /* For each entry of the array */
      for (index = 0; index * sizeof(long) < sym->st_size; index++)
	{

	  /* Read the array element */
	  elfsh_raw_read(file, 
			 elfsh_get_foffset_from_vaddr(file, vaddr + index * 4),
			 &loff, sizeof(long));
	  snprintf(buf, sizeof(buf), " %08X [foff: %u] \t %s[%0*u] = %08X", 
		   vaddr + index * sizeof(long), 
		   foffset + index * sizeof(long), 
		   name, 
		   ((sym->st_size / sizeof(long)) < 100  ? 2 : 
		    (sym->st_size / sizeof(long)) < 1000 ? 3 : 4),
		   index,
		   loff);

	  /* Check if this array element if a valid pointer */
	  sect = elfsh_get_parent_section(file, loff, &sct_index);
	  
	  /* If no, print the resolved symbol or section */
	  if (sect == NULL || strcmp(sect->name, ELFSH_SECTION_NAME_RODATA) ||
	      !elfsh_raw_read(file, elfsh_get_foffset_from_vaddr(file, loff), 
			      str, sizeof(str) - 1))
	    {

	      s = elfsh_reverse_symbol(file, loff, &idx_bytes);
	      if (NULL == s || idx_bytes > 1000)
		s = elfsh_reverse_dynsymbol(file, loff, &idx_bytes);
	      if (NULL == s || idx_bytes > 1000)
		{
		  if (sect != NULL)
		    {
		      s = sect->name;
		      idx_bytes = sct_index;
		    }
		  else
		    idx_bytes = 0;
		}
	      if (idx_bytes)
		snprintf(str, sizeof(str), "%s + %u", s, idx_bytes);
	      else
		snprintf(str, sizeof(str), "<IRREVELANT VADDR>");
	      printf("%-75s %s \n", buf, str);
	    }
	  
	  /* If yes, print the pointed data too */
	  else
	    printf("%-75s \"%s\" \n", buf, str);
	}
    }



  /* We want asm + hexa output of the code */
  else if (otype == ELFSH_DISASM_VIEW)
#if defined(USE_LIBASM)
    while (index < size)
      index += display_instr(1, index, vaddr, foffset, size, name, index, buff);
#else
  printf(" [!] LIBASM unavailable in this version, cant disasm %s\n",
	 (name != NULL && *name ? name : ELFSH_NULL_STRING));
#endif



  /* We want hexa + ascii output of the data */
  else if (ELFSH_HEXA_VIEW == otype)
    {

      if (name == NULL || !*name)
	name = ELFSH_NULL_STRING;

      while (index < size)
	{

	  /* Take care of quiet mode */
	  if (world.state.vm_quiet)
	    {
	      sprintf(buf, " %08X %s + %u", 
		      vaddr + index, name, index);
	      printf("%-40s ", buf);
	    }
	  else
	    {
	      sprintf(buf, " %08X [foff: %08u] %s + %u", 
		      vaddr + index, foffset + index, name, index);
	      printf("%-60s ", buf);
	    }
	  fflush(stdout);

	  ret = (world.state.vm_quiet ? 8 : 16);

	  /* Print hexa */
	  for (loff = 0; loff < ret; loff++)
	    {
	      printf("%c%c ", 
		     (index + loff < size ? 
		      (base[(buff[index + loff] >> 4) & 0x0F]) : ' '),
		     (index + loff < size ? 
		      (base[(buff[index + loff] >> 0) & 0x0F]) : ' '));
	      fflush(stdout);
	    }

	  /* Print ascii */
	  for (loff = 0; loff < ret; loff++)
	    {
	      putchar(index + loff >= size ? ' ' : 
		      PRINTABLE(buff[index + loff]) ? buff[index + loff] : '.');
	      fflush(stdout);
	    }

	  puts("");
	  index += ret;

	}
    }

  free(buff);
  puts("");
  return (0);
}









/* Print all variables and functions of the section */
int		display_section(elfshobj_t	*file, 
				elfshsect_t	*s, 
				char		*name, 
				u_int		off, 
				u_int		reqsize, 
				char		otype)
{
  Elf32_Sym	*actual;
  int		size;
  int		index;
  int		offset;
  int		tot;
  char		*symname;

  /* Hello message ;) */
  printf(" [*] Analysing section %s [*] \n\n", name);
  actual = elfsh_get_symtab(file, &size);
  tot = 0;
  if (s && !s->data)
    elfsh_get_anonymous_section(file, s);
  
  /* Display all symbols pointing in the section */
  if (actual)
    for (index = 0; index < size; index++)
      if (elfsh_get_parent_section(file, actual[index].st_value, &offset) == s)
	{
	  symname = elfsh_get_symbol_name(file, actual + index);
	  display_object(file, actual + index, 
			   (reqsize ? reqsize : actual[index].st_size), off,
			   s->shdr->sh_offset + actual[index].st_value - s->shdr->sh_addr,
			   actual[index].st_value, symname, otype);

	  tot++;
	}

  /* If no symbol points to our section, we display it as a whole */
  if (!tot)
    {
      actual = elfsh_get_symbol_by_name(file, name);
      display_object(file, actual, (reqsize ? reqsize : s->shdr->sh_size), 
		     off, s->shdr->sh_offset, s->shdr->sh_addr, name, otype);
    }
  return (0);
}










/* Display ASM code for a given function, or every functions of a section */
int             cmd_disasm()
{
  elfshlist_t	*actual;
  elfshsect_t	*s;
  elfshobj_t	*file;
  Elf32_Sym	*sym;
  char		*name;
  int		index;
  int		off;
  int		matchs;
  u_int		vaddr;
  char		special = 0; /* foffset or vaddr form */

  elfsh_get_symtab(world.current, NULL);
  elfsh_get_dynsymtab(world.current, NULL);
  puts("");

  /* now walk the regex list for this option */
  actual = &world.args.disasm;
  matchs = vaddr = 0;
  file = world.current;

  /* If the regex contains a vaddr instead of a symbol name */
  if (IS_VADDR(actual->rname))
    {
      if (sscanf(actual->rname + 2, "%X", &vaddr) != 1)
	ELFSH_SETERROR("[elfsh:cmd_disasm] Invalid virtual address requested\n", 
		       -1);
      special = 1;
    }
  
  /* else if it contains a file offset */
  else if (isdigit((int) *actual->rname))
    {
      vaddr = elfsh_get_vaddr_from_foffset(file, atoi(actual->rname));
      if (vaddr == 0xFFFFFFFF && sscanf(actual->rname + 2, "%X", &vaddr) != 1)
	ELFSH_SETERROR("[elfsh:cmd_disasm] Invalid file offset requested\n",
		       -1);
      special = 1;
    }
  
  /* If one of those special regex matched */
  if (special)
    {
      sym = elfsh_get_symbol_by_value(file, vaddr, &off,
				      ELFSH_LOWSYM);
      if (sym == NULL)
	sym = elfsh_get_symbol_by_value(file, vaddr, &off,
					ELFSH_HIGHSYM);
      if (sym == NULL)
	ELFSH_SETERROR("[elfsh] No matching symbol for offset\n", -1);
      actual->off += off;
      if (!actual->size)
	actual->size = elfsh_get_symbol_size(sym);
      actual->size += actual->off;
      name = elfsh_get_symbol_name(file, sym);
      matchs++;

#if __DEBUG_DISASM__
      fprintf(stderr, 
	      "[debug_disasm:cmd_disasm] Found special regx (%s) \n", 
	      name);
#endif

      display_object(file, sym, actual->size, actual->off,
		       elfsh_get_foffset_from_vaddr(file, vaddr),
		       vaddr, name, actual->otype);
      return (0);
    }
  
  
  /* Now find matches in the section header table (SHT) */
  for (s = file->sectlist; s != NULL; s = s->next)
    {
      name = s->name;
      if (name == NULL || !*name || s->shdr->sh_addr != NULL || 
	  regexec(&actual->name, name, 0, 0, 0))
	continue;
      matchs++;
      
#if __DEBUG_DISASM__
      fprintf(stderr, 
	      "[debug_disasm:cmd_disasm] Found section regx (%s) \n", 
	      name);
#endif
      
      display_section(file, s, name, actual->off, 
		      actual->size + actual->off, 
		      actual->otype);
    }
  
  
  /* Now find matches in the symbol table .symtab */
  if (file->secthash[ELFSH_SECTION_SYMTAB] != NULL)
    for (sym = file->secthash[ELFSH_SECTION_SYMTAB]->altdata, index = 0; 
	 index < file->secthash[ELFSH_SECTION_SYMTAB]->shdr->sh_size / sizeof(Elf32_Sym); 
	 index++)
      {
	name = elfsh_get_symbol_name(file, sym + index);
	if (name != NULL && *name && DUMPABLE(sym + index) &&
	    !regexec(&actual->name, name, 0, 0, 0))
	  {
	    matchs++;
	    if (!actual->size)
	      actual->size = elfsh_get_symbol_size(sym + index);
	    actual->size += actual->off;

#if __DEBUG_DISASM__
	    fprintf(stderr, 
		    "[debug_disasm:cmd_disasm] Found symbol regx (%s) \n",
		    name);
#endif

	    display_object(file, sym + index, actual->size, actual->off,
			     elfsh_get_foffset_from_vaddr(file, sym[index].st_value),
			     sym[index].st_value, name, actual->otype);
	  }
      }
  

  /* Now find matches in the dynamic symbol table .dynsym */
  if (file->secthash[ELFSH_SECTION_DYNSYM] != NULL)
    for (sym = file->secthash[ELFSH_SECTION_DYNSYM]->altdata, index = 0; 
	 index < file->secthash[ELFSH_SECTION_DYNSYM]->shdr->sh_size / sizeof(Elf32_Sym); 
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

#if __DEBUG_DISASM__
	    fprintf(stderr, 
		    "[debug_disasm:cmd_disasm] Found dynsym regx (%s) \n",
		    name);
#endif

	    display_object(file, sym + index, 
			   actual->size, actual->off,
			   elfsh_get_foffset_from_vaddr(file, sym[index].st_value),
			   sym[index].st_value, name, actual->otype);
	  }
      }
  
  /* Printing a warning message if we have no match */
  if (!world.state.vm_quiet && !matchs)
    fprintf(stderr, "[elfsh] No match for request %s\n\n", actual->rname);
  return (0);
}
