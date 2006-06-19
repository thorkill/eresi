/*
** symbol.c for libelfsh
**
** Started on  Mon Feb 26 04:11:46 2001 mayhem
*/
#include "libelfsh.h"




/*
** Return the symbol name giving its index in the symbol string table
** No special case for SECTION symbol because the section names strings
** have been duplicated in the symbol table.
*/
char		*elfsh_get_symbol_name(elfshobj_t *file, elfsh_Sym *s)
{
  void		*data;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || s == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Load symtab if needed */
  if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL &&
      elfsh_get_symtab(file, NULL) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot retreive symbol table", NULL);

  /* Else use the symbol string table */
  data = file->secthash[ELFSH_SECTION_STRTAB]->data;
  if ((NULL == file->secthash[ELFSH_SECTION_STRTAB] || NULL == data) && 
      NULL == elfsh_get_strtab(file, 0))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get STRTAB", NULL);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ((char *) data + s->st_name));
}




/* Return the used offset in .strtab or -1 if failed */
int			elfsh_set_symbol_name(elfshobj_t	*file,
					      elfsh_Sym		*s,
					      char		*name)
{
  elfshsect_t		*sct;
  char			*str;
  u_int			len;
  u_int			new_len;
  void			*data;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || s == NULL || name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL &&
      elfsh_get_symtab(file, NULL) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive symbol table", -1);

  /* Also change the section name in .shstrtab if symbol is STT_SECTION */
  if (elfsh_get_symbol_type(s) == STT_SECTION)
    {
      sct = elfsh_get_section_from_sym(file, s);
      if (sct != NULL && elfsh_set_section_name(file, sct, name) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to get section from its symbol", -1);
    }


  /* Else use the symbol string table */
  data = file->secthash[ELFSH_SECTION_STRTAB];
  if (file->secthash[ELFSH_SECTION_STRTAB] == NULL || data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No STRTAB available", NULL);

  /* Change the name */
  str = data;
  str += s->st_name;
  len = strlen(str);
  new_len = strlen(name);

  /* Do not allocate new place if possible */
  if (len >= new_len)
    strcpy(str, name);
  /* Append the name to .strtab */
  else
    s->st_name = elfsh_insert_in_strtab(file, name);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->st_name));
}



/* Return a ptr on the symbol table */
void		*elfsh_get_symtab(elfshobj_t *file, int *num)
{
  elfshsect_t	*s;
  int		strindex;
  int		index;
  int		nbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);

  else if (NULL == file->sht && NULL == elfsh_get_sht(file, NULL))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SHT", NULL);

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
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to load SYMTAB", NULL);
	  s->curend = s->shdr->sh_size;

	  /* Now load the string table */
	  s = elfsh_get_strtab(file, s->shdr->sh_link);
	  if (NULL == s)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to load STRTAB", NULL);	  
	  s->parent = file;
	}

      /*
      ** Fix 0 lenght syms and STT_SECTION syms
      ** Create a minimal .symtab if unexistant
      */
      elfsh_fixup_symtab(file, &strindex);
    }

  if (num != NULL)
    *num =
      file->secthash[ELFSH_SECTION_SYMTAB]->curend / sizeof(elfsh_Sym);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (file->secthash[ELFSH_SECTION_SYMTAB]->data));
}


/*
** Return the dynamic symbol name giving its value,
** Fill 'offset' with the difference between sym->st_value and 'value'
*/
char		*elfsh_reverse_symbol(elfshobj_t	*file,
				      elfsh_Addr       	value,
				      elfsh_SAddr      	*offset)
{
  elfshsect_t	*sect;
  elfsh_Sym	*sorted;
  int		num;
  int		index;
  char		*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (!value || value == 0xFFFFFFFF)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);
  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* handle dynamic case */
  if (elfsh_is_debug_mode())
    value -= file->rhdr.base;

  /* If there is no symtab, resolve using SHT */
  if (elfsh_get_symtab(file, &num) == NULL)
    {
      sect = elfsh_get_parent_section(file, value, offset);
      if (sect == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No parent section", NULL);

      *offset = (elfsh_SAddr) (sect->shdr->sh_addr - value);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_section_name(file, sect)));
    }

  /* Else use the sorted-by-address symbol table to match what we want */
  if (file->secthash[ELFSH_SECTION_SYMTAB]->altdata == NULL)
    elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]);
  sorted = file->secthash[ELFSH_SECTION_SYMTAB]->altdata;


  for (index = 0; index < num; index++)
    if (sorted[index].st_value <= value && DUMPABLE(sorted + index) &&
	(index + 1 >= num || sorted[index + 1].st_value > value))
      {
	*offset = (elfsh_SAddr) (value - sorted[index].st_value);
	str = elfsh_get_symbol_name(file, sorted + index);
	if (!*str)
	  str = NULL;
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (str));
      }

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "No valid symbol interval", NULL);
}





/* Return the symbol entry giving its name */
elfsh_Sym	*elfsh_get_symbol_by_name(elfshobj_t *file, char *name)
{
  elfsh_Sym	*sym;
  int		idx;
  int		size;
  char		*actual;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  if (elfsh_get_symtab(file, &size) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SYMTAB", NULL);

  sym = (elfsh_Sym *) file->secthash[ELFSH_SECTION_SYMTAB]->data;
  for (idx = 0; idx < size; idx++)
    {
      actual = elfsh_get_symbol_name(file, sym + idx);
      if (actual != NULL && !strcmp(actual, name))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym + idx));
    }
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Symbol not found", NULL);
}



/* Shift usual symbols (mandatory on solaris) */
void		elfsh_shift_usualsyms(elfshsect_t *sect, elfsh_Sym *sym)
{
  elfsh_Sym	*end;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  //printf("Calling shift usual syms ! \n");

  /* Change _end if necessary (solaris) */
  end = elfsh_get_dynsymbol_by_name(sect->parent, "_end");
  if (end != NULL && sym->st_value + sym->st_size > end->st_value)
    {
      //printf("Shift _end! \n");
      end->st_value = sym->st_value + sym->st_size;
    }

  /* Change _END_ if necessary (solaris) */
  end = elfsh_get_dynsymbol_by_name(sect->parent, "_END_");
  if (end != NULL && sym->st_value + sym->st_size > end->st_value)
    {
      //printf("Shift _END_! \n");
      end->st_value = sym->st_value + sym->st_size;
    }

  /* Change _edata if necessary (solaris) */
  if (elfsh_get_ostype(sect->parent) == ELFSH_OS_SOLARIS)
    {
      end = elfsh_get_dynsymbol_by_name(sect->parent, "_edata");
      if (end != NULL && sym->st_value + sym->st_size > end->st_value)
	{
	  //printf("Shift _edata! \n");
	  end->st_value = sym->st_value + sym->st_size;
	}
    }
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Insert a symbol in the given symbol table */
/* This function is not e2dbg safe */
int		elfsh_insert_symbol(elfshsect_t *sect,
				    elfsh_Sym	*sym,
				    char	*name)
{
  elfsh_Sym	*orig;
  int		index;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (sect == NULL || sect->shdr == NULL ||
      (sect->shdr->sh_type != SHT_SYMTAB &&
       sect->shdr->sh_type != SHT_DYNSYM))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Input section is not SYMTAB", -1);
  if (name == NULL)
    name = ELFSH_NULL_STRING;

  /* Check if symbol already exists */
  orig = elfsh_get_symbol_by_name(sect->parent, name);
  if (orig != NULL && sym->st_value == orig->st_value)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       (((char *) orig - (char *) sect->data) /
			ELFSH_SYMTAB_ENTRY_SIZE));

  /* Shift some special symbols */
  //if (sect->shdr->sh_type == SHT_DYNSYM)
  elfsh_shift_usualsyms(sect, sym);

  /* Insert symbol name in .shstrtab */
  index = elfsh_insert_in_strtab(sect->parent, name);
  if (index < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to insert in SHSTRTAB", -1);

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Injected symbol %-20s [" AFMT "] \n",
	 name, (elfsh_Addr) sym->st_value);
#endif

  /* Insert symbol in .symtab */
  sym->st_name = index;
  index = elfsh_append_data_to_section(sect, sym, sizeof(elfsh_Sym));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (index));
}




/* Remove a symbol */
/* This function is not e2dbg safe */
int		elfsh_remove_symbol(elfshsect_t *symtab, char *name)
{
  elfsh_Sym	*ret;
  elfsh_Sym	*tab;
  elfsh_Sym	*new;
  u_int   	off;
  u_int		movedsz;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (symtab == NULL || name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);
  ret = elfsh_get_symbol_by_name(symtab->parent, name);
  if (ret == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown symbol", -1);

  /* Do it */
  tab = symtab->data;
  off = (u_long) ret - (u_long) tab;
  movedsz = symtab->shdr->sh_size - off - sizeof(elfsh_Sym);
  if (movedsz)
    memcpy((char *) symtab->data + off, 
	   (char *) symtab->data + off + sizeof(elfsh_Sym),
	   movedsz);
  symtab->shdr->sh_size -= sizeof(elfsh_Sym);
  symtab->curend -= sizeof(elfsh_Sym);
  XALLOC(new, symtab->shdr->sh_size, -1);
  memcpy(new, tab, symtab->shdr->sh_size);
  XFREE(tab);
  symtab->data = new;

  /* We just cant remove the string because of ELF string table format */
  elfsh_sync_sorted_symtab(symtab);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}







/* Retreive the file offset giving the virtual address */
int		elfsh_get_symbol_foffset(elfshobj_t *file, elfsh_Sym *sym)
{
  elfshsect_t	*sect;
  char		*name;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If the symbol is a section, then look at the sht instead */
  if (elfsh_get_symbol_type(sym) == STT_SECTION)
    {
      name = elfsh_get_symbol_name(file, sym);
      sect = elfsh_get_section_by_name(file, name, NULL, NULL, NULL);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (sect ? elfsh_get_section_foffset(sect->shdr) : 0));
    }

  /* get our parent section and compute the file offset */
  if (sym == NULL || file == NULL || NULL == sym->st_value)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  sect = elfsh_get_parent_section(file, sym->st_value, NULL);
  if (sect == NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (sect->shdr->sh_offset + 
		      (sym->st_value - sect->shdr->sh_addr)));
}



/* Get symtab entry by vaddr */
elfsh_Sym	  *elfsh_get_symbol_by_value(elfshobj_t	*file,
					     elfsh_Addr	vaddr,
					     int	*off,
					     int	mode)
{
  int		  num;
  elfsh_Sym	  *data;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_symtab(file, &num);
  if (data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive SYMTAB", NULL);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_get_sym_by_value(data, num, vaddr, off, mode)));
}



/* Shift the symbol table */
int		elfsh_shift_symtab(elfshobj_t *file, elfsh_Addr limit, int inc)
{
  elfshsect_t	*actual;
  int		err;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  actual = elfsh_get_section_by_type(file, SHT_SYMTAB, 0, NULL, NULL, NULL);
  if (actual == NULL || actual->data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to find SYMTAB by type", -1);

  err = elfsh_shift_syms(file, actual, limit, inc);
  if (err < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to shift SYMTAB", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Insert STT_SECTION symbol */
int		elfsh_insert_sectsym(elfshobj_t *file, elfshsect_t *sect)
{
  elfsh_Sym	new;
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  new = elfsh_create_symbol(sect->shdr->sh_addr, sect->curend,
			    STT_SECTION, STB_LOCAL, 0, sect->index);
  ret = elfsh_insert_symbol(file->secthash[ELFSH_SECTION_SYMTAB],
			    &new, sect->name);
  if (ret < 0)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
  ret = elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]);

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Inserted STT_SECT symbol %s [" AFMT "] sz:%u cur:%u \n",
	 sect->name, (elfsh_Addr) sect->shdr->sh_addr, 
	 sect->shdr->sh_size, sect->curend);
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}


/* Insert STT_SECTION symbol */
int		elfsh_insert_funcsym(elfshobj_t *file, char *name, 
				     elfsh_Addr vaddr, 
				     uint32_t sz, uint32_t sctidx)
{
  elfsh_Sym	new;
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  new = elfsh_create_symbol(vaddr, sz, STT_FUNC, STB_LOCAL, 0, sctidx);
  ret = elfsh_insert_symbol(file->secthash[ELFSH_SECTION_SYMTAB],
			    &new, name);
  if (ret < 0)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
  ret = elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]);

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Inserted STT_FUNC symbol %s [" AFMT 
	 "] sz:%u sctidx:%u \n",
	 name, vaddr, sz, sctidx);
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}
