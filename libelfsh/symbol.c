/*
** symbol.c for libelfsh
**
** Started on  Mon Feb 26 04:11:46 2001 mayhem
** Last update Fri Aug 15 23:46:54 2003 jv
*/
#include "libelfsh.h"




/*
** Return the symbol name giving its index in the symbol string table
** No special case for SECTION symbol because the section names strings
** have been duplicated in the symbol table.
*/
char		*elfsh_get_symbol_name(elfshobj_t *file, Elf32_Sym *s)
{
  char		*str;

  /* Sanity checks */
  if (file == NULL || s == NULL)
    ELFSH_SETERROR("[libelfsh:get_symbol_name] Invalid NULL parameter\n", NULL);

  /* Load symtab if needed */
  if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL &&
      elfsh_get_symtab(file, NULL) == NULL)
    ELFSH_SETERROR("[libelfsh:get_symbol_name] Cannot retreive symbol table\n",
		   NULL);

  /* Else use the symbol string table */
  if ((NULL == file->secthash[ELFSH_SECTION_STRTAB] ||
       NULL == file->secthash[ELFSH_SECTION_STRTAB]->data) &&
      NULL == elfsh_get_strtab(file, 0))
    return (NULL);
  str = file->secthash[ELFSH_SECTION_STRTAB]->data;
  return (str + s->st_name);
}




/* Return the used offset in .strtab or -1 if failed */
int			elfsh_set_symbol_name(elfshobj_t	*file,
					      Elf32_Sym		*s,
					      char		*name)
{
  elfshsect_t		*sct;
  char			*str;
  u_int			len;
  u_int			new_len;

  /* Sanity checks */
  if (file == NULL || s == NULL || name == NULL)
    ELFSH_SETERROR("[libelfsh:set_symbol_name] Invalid NULL parameter", -1);
  if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL &&
      elfsh_get_symtab(file, NULL) == NULL)
    ELFSH_SETERROR("[libelfsh:get_symbol_name] Cannot retreive symbol table",
		   -1);

  /* Also change the section name in .shstrtab if symbol is STT_SECTION */
  if (elfsh_get_symbol_type(s) == STT_SECTION)
    {
      sct = elfsh_get_section_from_sym(file, s);
      if (sct != NULL && elfsh_set_section_name(file, sct, name) < 0)
	return (-1);
    }


  /* Else use the symbol string table */
  if (file->secthash[ELFSH_SECTION_STRTAB] == NULL ||
      file->secthash[ELFSH_SECTION_STRTAB]->data == NULL)
    return (NULL);

  /* Change the name */
  str = file->secthash[ELFSH_SECTION_STRTAB]->data;
  str += s->st_name;
  len = strlen(str);
  new_len = strlen(name);

  /* Do not allocate new place if possible */
  if (len >= new_len)
    strcpy(str, name);
  /* Append the name to .strtab */
  else
    s->st_name = elfsh_insert_in_strtab(file, name);

  return (s->st_name);
}



/* Return a ptr on the symbol table */
void		*elfsh_get_symtab(elfshobj_t *file, int *num)
{
  elfshsect_t	*s;
  int		strindex;
  int		index;
  int		nbr;

  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_symtab] Invalid NULL parameter\n", NULL);
  else if (NULL == file->sht && NULL == elfsh_get_sht(file, NULL))
    return (NULL);

  if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL)
    {

      /* If symtab is already loaded, return it */
      s = elfsh_get_section_by_type(file, SHT_SYMTAB,
				    0, &index,
				    &strindex, &nbr);
      if (s != NULL)
	{
	  file->secthash[ELFSH_SECTION_SYMTAB] = s;
	  s->data = elfsh_load_section(file, s->shdr);
	  if (s->data == NULL)
	    return (NULL);

	  /* Now load the string table */
	  s = elfsh_get_strtab(file, s->shdr->sh_link);
	}

      /*
      ** Fix 0 lenght syms and STT_SECTION syms
      ** Create a minimal .symtab if unexistant
      */
      elfsh_fixup_symtab(file, &strindex);
    }

  if (num != NULL)
    *num =
      file->secthash[ELFSH_SECTION_SYMTAB]->shdr->sh_size / sizeof(Elf32_Sym);

  return (file->secthash[ELFSH_SECTION_SYMTAB]->data);
}






/*
** Return the dynamic symbol name giving its value,
** Fill 'offset' with the difference between sym->st_value and 'value'
*/
char		*elfsh_reverse_symbol(elfshobj_t	*file,
				      u_int		value,
				      int		*offset)
{
  elfshsect_t	*sect;
  Elf32_Sym	*sorted;
  int		num;
  int		index;
  char		*str;

  /* Sanity checks */
  if (!value || value == 0xFFFFFFFF)
    return (NULL);
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:reverse_symbol] Invalid NULL parameter\n", NULL);

  /* If there is no symtab, resolve using SHT */
  if (elfsh_get_symtab(file, &num) == NULL)
    {
      sect = elfsh_get_parent_section(file, value, offset);
      if (sect == NULL)
	ELFSH_SETERROR("[libelfsh:reverse_symbol] No parent section\n", NULL);
      *offset = (int) sect->shdr->sh_addr - value;
      return (elfsh_get_section_name(file, sect));
    }

  /* Else use the sorted-by-address symbol table to match what we want */
  if (file->secthash[ELFSH_SECTION_SYMTAB]->altdata == NULL)
    elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]);
  sorted = file->secthash[ELFSH_SECTION_SYMTAB]->altdata;

  for (index = 0; index < num; index++)
    if (sorted[index].st_value <= value && DUMPABLE(sorted + index) &&
	(index + 1 >= num || sorted[index + 1].st_value > value))
      {
	*offset = (int) (value - sorted[index].st_value);
	str = elfsh_get_symbol_name(file, sorted + index);
	if (!*str)
	  str = NULL;
	return (str);
      }

  ELFSH_SETERROR("[libelfsh:reverse_symbol] No valid symbol interval\n", NULL);
}





/* Return the symbol entry giving its name */
Elf32_Sym	*elfsh_get_symbol_by_name(elfshobj_t *file, char *name)
{
  Elf32_Sym	*sym;
  int		idx;
  int		size;
  char		*actual;

  if (file == NULL || name == NULL)
    ELFSH_SETERROR("[libelfsh:get_symbol_by_name] Invalid NULL parameter\n",
		   NULL);
  if (elfsh_get_symtab(file, &size) == NULL)
    return (NULL);
  sym = file->secthash[ELFSH_SECTION_SYMTAB]->data;
  for (idx = 0; idx < size; idx++)
    {
      actual = elfsh_get_symbol_name(file, sym + idx);
      if (actual != NULL && !strcmp(actual, name))
	return (sym + idx);
    }
  ELFSH_SETERROR("[libelfsh:get_symbol_by_name] Symbol not found\n",
		 NULL);
}



/* Shift usual symbols (mandatory on solaris) */
void		elfsh_shift_usualsyms(elfshsect_t *sect, Elf32_Sym *sym)
{
  Elf32_Sym	*end;

  /* Change _end if necessary (solaris) */
  end = elfsh_get_dynsymbol_by_name(sect->parent, "_end");
  if (end != NULL && sym->st_value + sym->st_size > end->st_value)
    end->st_value = sym->st_value + sym->st_size;

  /* Change _END_ if necessary (solaris) */
    end = elfsh_get_dynsymbol_by_name(sect->parent, "_END_");
  if (end != NULL && sym->st_value + sym->st_size > end->st_value)
    end->st_value = sym->st_value + sym->st_size;

  /* Change _edata if necessary (solaris) */
  end = elfsh_get_dynsymbol_by_name(sect->parent, "_edata");
  if (end != NULL && sym->st_value + sym->st_size > end->st_value)
    end->st_value = sym->st_value + sym->st_size;

  /* Change _end if necessary (solaris) */
  end = elfsh_get_symbol_by_name(sect->parent, "_end");
  if (end != NULL && sym->st_value + sym->st_size > end->st_value)
    end->st_value = sym->st_value + sym->st_size;

  /* Change _END_ if necessary (solaris) */
    end = elfsh_get_symbol_by_name(sect->parent, "_END_");
  if (end != NULL && sym->st_value + sym->st_size > end->st_value)
    end->st_value = sym->st_value + sym->st_size;

  /* Change _edata if necessary (solaris) */
  end = elfsh_get_symbol_by_name(sect->parent, "_edata");
  if (end != NULL && sym->st_value + sym->st_size > end->st_value)
    end->st_value = sym->st_value + sym->st_size;
}


/* Insert a symbol in the given symbol table */
int		elfsh_insert_symbol(elfshsect_t *sect,
				    Elf32_Sym	*sym,
				    char	*name)
{
  Elf32_Sym	*orig;
  int		index;

  /* Sanity checks */
  if (sect == NULL || sect->shdr == NULL ||
      (sect->shdr->sh_type != SHT_SYMTAB &&
       sect->shdr->sh_type != SHT_DYNSYM))
    ELFSH_SETERROR("[libelfsh:insert_symbol] Input section is not SYMTAB\n",
		   -1);
  if (name == NULL)
    name = ELFSH_NULL_STRING;

  /* Check if symbol already exists */
  orig = elfsh_get_symbol_by_name(sect->parent, name);
  if (orig != NULL && sym->st_value == orig->st_value)
    return (((u_int) orig - (u_int) sect->data) / ELFSH_SYMTAB_ENTRY_SIZE);

  /* Shift some special symbols */
  elfsh_shift_usualsyms(sect, sym);

  /* Insert symbol name in .shstrtab */
  index = elfsh_insert_in_strtab(sect->parent, name);
  if (index < 0)
    return (-1);

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Injected symbol %-20s [%08X] \n",
	 name, (u_int) sym->st_value);
#endif

  /* Insert symbol in .symtab */
  sym->st_name = index;
  index = elfsh_append_data_to_section(sect, sym, sizeof(Elf32_Sym));
  return (index);
}





/* Retreive the file offset giving the virtual address */
int		elfsh_get_symbol_foffset(elfshobj_t *file, Elf32_Sym *sym)
{
  elfshsect_t	*sect;
  char		*name;

  /* If the symbol is a section, then look at the sht instead */
  if (elfsh_get_symbol_type(sym) == STT_SECTION)
    {
      name = elfsh_get_symbol_name(file, sym);
      sect = elfsh_get_section_by_name(file, name, NULL, NULL, NULL);
      return (sect ? elfsh_get_section_foffset(sect->shdr) : 0);
    }

  /* get our parent section and compute the file offset */
  if (sym == NULL || file == NULL || NULL == sym->st_value)
    return (0);
  sect = elfsh_get_parent_section(file, sym->st_value, NULL);
  if (sect == NULL)
    return (0);
  return (sect->shdr->sh_offset + (sym->st_value - sect->shdr->sh_addr));
}



/* Get symtab entry by vaddr */
Elf32_Sym	  *elfsh_get_symbol_by_value(elfshobj_t	*file,
					     u_int	vaddr,
					     int	*off,
					     int	mode)
{
  int		  num;

  if (NULL == elfsh_get_symtab(file, &num))
    ELFSH_SETERROR("[libelfsh:get_symbol_by_value] Cannot retreive SYMTAB\n",
		   NULL);

  return (elfsh_get_sym_by_value(file->secthash[ELFSH_SECTION_SYMTAB]->data,
				 num, vaddr, off, mode));
}



/* Shift the symbol table */
int		elfsh_shift_symtab(elfshobj_t *file, u_long limit, int inc)
{
  elfshsect_t	*actual;
  u_int		nbr;
  u_int		idx;
  Elf32_Sym	*sym;

#if __DEBUG_RELADD__
  static u_int	totshift = 0;
  totshift += inc;
  printf("[DEBUG_RELADD] Shifting symtab [LIM %08X .::. INC %08u .::. TOT %08u] \n",
	 (u_int) limit, inc, totshift);
#endif

  actual = elfsh_get_section_by_type(file, SHT_SYMTAB, 0, NULL, NULL, &nbr);
  if (actual == NULL || actual->data == NULL || !nbr)
    return (-1);
  sym = actual->data;

  for (idx = 0; idx < (nbr / ELFSH_SYMTAB_ENTRY_SIZE); idx++)
    if (sym[idx].st_value >= limit && limit != 0)
      {

#if __DEBUG_RELADD__
	printf("[DEBUG_RELADD] Shifted symbol *%-20s* [%08X -> %08X]\n",
	       elfsh_get_symbol_name(file, sym),
	       (u_int) sym[idx].st_value,
	       (u_int) sym[idx].st_value + inc);
#endif

	sym[idx].st_value += inc;
      }

  elfsh_sync_sorted_symtab(actual);
  return (0);
}



/* Insert STT_SECTION symbol */
int		elfsh_insert_sectsym(elfshobj_t *file, elfshsect_t *sect)
{
  Elf32_Sym	new;
  int		ret;

  new = elfsh_create_symbol(sect->shdr->sh_addr, sect->shdr->sh_size, STT_SECTION,
			    STB_LOCAL, 0, sect->index);
  ret = elfsh_insert_symbol(file->secthash[ELFSH_SECTION_SYMTAB],
			    &new, sect->name);
  elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]);

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Inserted STT_SECT symbol %s [%08X] \n",
	 sect->name, (u_int) sect->shdr->sh_addr);
#endif

  return (ret);
}
