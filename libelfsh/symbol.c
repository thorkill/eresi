/**
 * @file symbol.c
 * @ingroup libelfsh
 * symbol.c for libelfsh
 * 
 * Started on  Mon Feb 26 04:11:46 2001 jfv
 *
 * $Id: symbol.c,v 1.17 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libelfsh.h"


#define ELFSH_SYMTAB_HASH_NAME 	"elfsh_symtab_hashbyname"

/**
 * Return the symbol name giving its index in the symbol string table
 * No special case for SECTION symbol because the section names strings
 * have been duplicated in the symbol table.
 * @param file
 * @param s
 * @return
 */
char		*elfsh_get_symbol_name(elfshobj_t *file, elfsh_Sym *s)
{
  elfshsect_t	*sect;
  void		*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || s == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Load symtab if needed */
  if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL &&
      elfsh_get_symtab(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot retreive symbol table", NULL);

  /* Else use the symbol string table */
  sect = file->secthash[ELFSH_SECTION_STRTAB];
  if (!sect || !sect->data)
    {
      sect = elfsh_get_strtab(file, 0);
      if (!sect)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to get STRTAB", NULL);
      data = sect->data;
    }
  else
    data = sect->data;
  
  /* A last check to avoid getting killed on corrupted symbol tables */
  if (!data)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, "<unreadable>");
  else if ((unsigned int) s->st_name > 
	   (unsigned int) file->secthash[ELFSH_SECTION_STRTAB]->shdr->sh_size)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, "<corrupted>");
  
  /* Return name */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ((char *) data + s->st_name));
}



/**
 * Return the used offset in .strtab or -1 if failed 
 * @param file
 * @param s
 * @param name
 * @return
 */
int			elfsh_set_symbol_name(elfshobj_t	*file,
					      elfsh_Sym		*s,
					      char		*name)
{
  elfshsect_t		*sct;
  char			*str;
  unsigned int			len;
  unsigned int			new_len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || s == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL &&
      elfsh_get_symtab(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive symbol table", -1);

  /* Also change the section name in .shstrtab if symbol is STT_SECTION */
  if (elfsh_get_symbol_type(s) == STT_SECTION)
    {
      sct = elfsh_get_section_from_sym(file, s);
      if (sct != NULL && elfsh_set_section_name(file, sct, name) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to get section from its symbol", -1);
    }


  /* Else use the symbol string table */
  if (file->secthash[ELFSH_SECTION_STRTAB]       == NULL || 
      file->secthash[ELFSH_SECTION_STRTAB]->data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No STRTAB available", NULL);

  /* Change the name */
  str = file->secthash[ELFSH_SECTION_STRTAB]->data;
  str += s->st_name;
  len = strlen(str);
  new_len = strlen(name);

  /* Do not allocate new place if possible */
  if (len >= new_len)
    strncpy(str, name, new_len);
  /* Append the name to .strtab */
  else
    s->st_name = elfsh_insert_in_strtab(file, name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->st_name));
}



/**
 * Return a ptr on the symbol table
 * @param file
 * @param num
 * @return
 */
void		*elfsh_get_symtab(elfshobj_t *file, int *num)
{
  elfshsect_t	*s;
  int		strindex;
  int		index;
  int		nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);

  else if (NULL == file->sht && NULL == elfsh_get_sht(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SHT", NULL);

  if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL)
    {
      //fprintf(stderr, "Loading symtab for object %s \n", file->name);

      /* If symtab is already loaded, return it */
      s = elfsh_get_section_by_type(file, SHT_SYMTAB,
				    0, &index,
				    &strindex, &nbr);
      if (s != NULL)
	{
	  file->secthash[ELFSH_SECTION_SYMTAB] = s;
	  s->data = elfsh_load_section(file, s->shdr);
	  if (s->data == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to load SYMTAB", NULL);
	  s->curend = s->shdr->sh_size;

	  /* Now load the string table */
	  s = elfsh_get_strtab(file, s->shdr->sh_link);
	  if (NULL == s)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to load STRTAB", NULL);	  
	  s->parent = file;
	}

      /*
      ** Fix 0 lenght syms and STT_SECTION syms
      ** Create a minimal .symtab if unexistant
      */
      elfsh_fixup_symtab(file, &strindex);

      //fprintf(stderr, "symtab FIXED for object %s \n", file->name);
      
    }

  if (num != NULL)
    *num =
      file->secthash[ELFSH_SECTION_SYMTAB]->curend / sizeof(elfsh_Sym);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (file->secthash[ELFSH_SECTION_SYMTAB]->data));
}


/**
 * Return the dynamic symbol name giving its value,
 * Fill 'offset' with the difference between sym->st_value and 'value'
 * @param file
 * @param value
 * @param offset
 * @return
 */
char		*elfsh_reverse_symbol(elfshobj_t	*file,
				      eresi_Addr       	value,
				      elfsh_SAddr      	*offset)
{
  elfshsect_t	*sect;
  elfsh_Sym	*sorted;
  int		num;
  int		index;
  char		*str;
  int		best;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (!value || value == 0xFFFFFFFF)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* handle dynamic case */
  if (elfsh_is_debug_mode())
    value -= file->rhdr.base;

  /* If there is no symtab, resolve using SHT */
  if (elfsh_get_symtab(file, &num) == NULL)
    {
      sect = elfsh_get_parent_section(file, value, offset);
      if (sect == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No parent section", NULL);

      *offset = (elfsh_SAddr) (sect->shdr->sh_addr - value);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_section_name(file, sect)));
    }

  /* Else use the sorted-by-address symbol table to match what we want */
  if (file->secthash[ELFSH_SECTION_SYMTAB]->altdata == NULL)
    elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]);
  sorted = file->secthash[ELFSH_SECTION_SYMTAB]->altdata;

  /* Now find the best symbol -- type is more important than offset */
  for (str = NULL, best = index = 0; index < num; index++)
    if (sorted[index].st_value <= value && DUMPABLE(sorted + index))
      {
	if (best && !BESTYPE(sorted + index, sorted + best))
	  continue;

	*offset = (elfsh_SAddr) (value - sorted[index].st_value);
	best = index;
	str = elfsh_get_symbol_name(file, sorted + index);
	if (!*str)
	  str = NULL;
      }

  if (str)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, str);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "No valid symbol interval", NULL);
}

/**
 * Fill symtab and dynsym hash table for _get_*_by_name functions
 * @param file target file
 * @return
 */
int		elfsh_init_symbol_hashtables(elfshobj_t *file)
{
  elfsh_Sym	*sym;
  int		idx;
  int		size;
  char		*actual;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  // THIS FUNCTION IS DISABLED (TEST)
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);

  printf("Init symbol hash tables ! \n");

  hash_init(&file->symhash, ELFSH_SYMHASH_NAME, 100, ASPECT_TYPE_INT);
  hash_init(&file->dynsymhash, ELFSH_DYNSYMHASH_NAME, 100, ASPECT_TYPE_INT);

  /* Symtab */
  if (elfsh_get_symtab(file, &size))
    {
      sym = (elfsh_Sym *) file->secthash[ELFSH_SECTION_SYMTAB]->data;
      
      for (idx = 0; idx < size; idx++)
	{
	  actual = elfsh_get_symbol_name(file, sym + idx);
	  if (actual)
	    hash_add(&file->symhash, strdup(actual), (void *) idx);
	}
    }

  sym = (elfsh_Sym *) elfsh_get_dynsymtab(file, &size);
  
  /* Dynsym */
  if (sym)
    {
      for (idx = 0; idx < size; idx++)
	{
	  actual = elfsh_get_dynsymbol_name(file, sym + idx);
	  if (actual)
	    hash_add(&file->dynsymhash, strdup(actual), (void *) idx);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Return the symbol entry giving its name 
 * @param file target file
 * @param name symbol name
 * @return symbol pointer or NULL
 */
elfsh_Sym	*elfsh_get_symbol_by_name(elfshobj_t *file, char *name)
{
  elfsh_Sym	*sym;
  int		idx;
  int		size;
  char		*actual;
  elfshsect_t	*sect;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check arguments */
  if (file == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Setup symtab pointers */
  if (elfsh_get_symtab(file, &size) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get SYMTAB", NULL);

  sect = file->secthash[ELFSH_SECTION_SYMTAB];

  /* Invalid section pointer */
  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get SYMTAB (invalid section pointer)", NULL);

  sym = (elfsh_Sym *) sect->data;

  /* Invalid section data */
  if (sym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get SYMTAB data", NULL);

  /*
  if (file->symhash.ent)
    {
    //idx is the symbol number in the section
      idx = (int) hash_get(&file->symhash, name);

#if __DEBUG_HASH_BY_NAME__
      printf("[DEBUG_HASH_BY_NAME] SYM HASH Search by name for %s => %d\n",
	     name, idx);
#else
//Check if idx is in the section
      if (idx <= 0 || idx >= sect->shdr->sh_size)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Symbol not found", NULL);

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym + idx));
#endif
    }
*/
 

  for (idx = 0; idx < size; idx++)
    {
      actual = elfsh_get_symbol_name(file, sym + idx);
      if (actual && !strcmp(actual, name))
	{
#if __DEBUG_HASH_BY_NAME__
	  printf("[DEBUG_HASH_BY_NAME] SYM ITERATE Search by name for %s => %d\n", 
		 name, idx);
#endif
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym + idx));
	}
    }

#if __DEBUG_HASH_BY_NAME__
  printf("[DEBUG_HASH_BY_NAME] SYM ITERATE Search by name for %s => NOT FOUND\n", name);
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	       "Symbol not found", NULL);
}



/**
 * Shift usual symbols (mandatory on solaris)
 * @param sect
 * @param sym
 */
void		elfsh_shift_usualsyms(elfshsect_t *sect, elfsh_Sym *sym)
{
  elfsh_Sym	*end;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //fprintf(stderr, "Calling shift usual syms ! \n");

  /* Change _end in the symbol table */
  end = elfsh_get_symbol_by_name(sect->parent, "_end");
  if (end != NULL && sym->st_value + sym->st_size > end->st_value)
    {
      //printf("Shift _end! \n");
      end->st_value = sym->st_value + sym->st_size;
    }

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
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/**
 * Insert a symbol in the given symbol table 
 * This function is not e2dbg safe 
 * @param sect
 * @param sym
 * @param name
 * @return
 */
int		elfsh_insert_symbol(elfshsect_t *sect,
				    elfsh_Sym	*sym,
				    char	*name)
{
  elfsh_Sym	*orig;
  int		index;
  int		mode;
  hash_t	*uptable;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //fprintf(stderr, "Adding symbol %s \n", name);

  /* Sanity checks */
  if (sect == NULL || sect->shdr == NULL ||
      (sect->shdr->sh_type != SHT_SYMTAB &&
       sect->shdr->sh_type != SHT_DYNSYM))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Input section is not SYMTAB", -1);
  if (name == NULL)
    name = ELFSH_NULL_STRING;

  /* Check if symbol already exists */
  orig = elfsh_get_symbol_by_name(sect->parent, name);
  if (orig != NULL && sym->st_value == orig->st_value)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       (((char *) orig - (char *) sect->data) /
			ELFSH_SYMTAB_ENTRY_SIZE));

  /* Shift some special symbols */
  //if (sect->shdr->sh_type == SHT_DYNSYM)
  //fprintf(stderr, "Shifting usual symbols\n");
  mode = elfsh_get_mode();
  elfsh_set_static_mode();
  elfsh_shift_usualsyms(sect, sym);
  elfsh_set_mode(mode);
  //fprintf(stderr, "Shifted usual symbols\n");

  /* Insert symbol name in .shstrtab */
  index = elfsh_insert_in_strtab(sect->parent, name);
  if (index < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to insert in SHSTRTAB", -1);

#if __DEBUG_RELADD__
  fprintf(stderr, "[DEBUG_RELADD] Injected symbol %-20s [" AFMT "] \n",
	  name, (eresi_Addr) sym->st_value);
#endif

  /* Insert symbol in .symtab */
  sym->st_name = index;
  index = elfsh_append_data_to_section(sect, sym, sizeof(elfsh_Sym));

  /* Update hashtable */
  switch(sect->shdr->sh_type)
    {
    case SHT_SYMTAB:
      uptable = &sect->parent->symhash;
      break;
    case SHT_DYNSYM:
      uptable = &sect->parent->dynsymhash;
      break;
    default:
      uptable = NULL;
      break;
    }

  if (uptable && uptable->ent)
    hash_add(uptable, strdup(name), (void *) index);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sym->st_name);
}




/**
 * Remove a symbol
 * This function is not e2dbg safe
 * @param symtab
 * @param name
 * @return
 */
int		elfsh_remove_symbol(elfshsect_t *symtab, char *name)
{
  elfsh_Sym	*ret;
  elfsh_Sym	*tab;
  elfsh_Sym	*enew;
  unsigned int   	off;
  unsigned int		movedsz;
  hash_t 	*uptable = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (symtab == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);
  ret = elfsh_get_symbol_by_name(symtab->parent, name);
  if (ret == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown symbol", -1);

  /* Do it */
  tab = symtab->data;
  off = (unsigned long) ret - (unsigned long) tab;
  movedsz = symtab->shdr->sh_size - off - sizeof(elfsh_Sym);
  if (movedsz)
    memcpy((char *) symtab->data + off, 
	   (char *) symtab->data + off + sizeof(elfsh_Sym),
	   movedsz);
  symtab->shdr->sh_size -= sizeof(elfsh_Sym);
  symtab->curend -= sizeof(elfsh_Sym);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,enew, symtab->shdr->sh_size, -1);
  memcpy(enew, tab, symtab->shdr->sh_size);
  XFREE(__FILE__, __FUNCTION__, __LINE__,tab);
  symtab->data = enew;

  /* We just cant remove the string because of ELF string table format */
  elfsh_sync_sorted_symtab(symtab);

  /* Update hashtable */
  switch(symtab->shdr->sh_type)
    {
    case SHT_SYMTAB:
      uptable = &symtab->parent->symhash;
      break;
    case SHT_DYNSYM:
      uptable = &symtab->parent->dynsymhash;
      break;
    }

  if (uptable && uptable->ent)
    hash_del(uptable, name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Retreive the file offset giving the virtual address
 * @param file
 * @param sym
 * @return
 */
int		elfsh_get_symbol_foffset(elfshobj_t *file, elfsh_Sym *sym)
{
  elfshsect_t	*sect;
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If the symbol is a section, then look at the sht instead */
  if (elfsh_get_symbol_type(sym) == STT_SECTION)
    {
      name = elfsh_get_symbol_name(file, sym);
      sect = elfsh_get_section_by_name(file, name, NULL, NULL, NULL);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (sect ? elfsh_get_section_foffset(sect->shdr) : 0));
    }

  /* get our parent section and compute the file offset */
  if (sym == NULL || file == NULL || NULL == sym->st_value)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  sect = elfsh_get_parent_section(file, sym->st_value, NULL);
  if (sect == NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (sect->shdr->sh_offset + 
		      (sym->st_value - sect->shdr->sh_addr)));
}

/**
 * Get symtab entry by vaddr 
 * @param file
 * @param vaddr
 * @param off
 * @param mode
 * @return
 */
elfsh_Sym	  *elfsh_get_symbol_by_value(elfshobj_t	*file,
					     eresi_Addr	vaddr,
					     int	*off,
					     int	mode)
{
  int		  num;
  elfsh_Sym	  *data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_symtab(file, &num);
  if (data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive SYMTAB", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_get_sym_by_value(data, num, vaddr, off, mode)));
}



/**
 * Shift the symbol table 
 * @param file
 * @param limit
 * @param inc
 * @return
 */
int		elfsh_shift_symtab(elfshobj_t *file, eresi_Addr limit, int inc)
{
  elfshsect_t	*actual;
  int		err;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  actual = elfsh_get_section_by_type(file, SHT_SYMTAB, 0, NULL, NULL, NULL);
  if (actual == NULL || actual->data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to find SYMTAB by type", -1);

  err = elfsh_shift_syms(file, actual, limit, inc);
  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to shift SYMTAB", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Insert STT_SECTION symbol 
 * @param file
 * @param sect
 * @return
 */
int		elfsh_insert_sectsym(elfshobj_t *file, elfshsect_t *sect)
{
  elfsh_Sym	new;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  new = elfsh_create_symbol(sect->shdr->sh_addr, sect->curend,
			    STT_SECTION, STB_LOCAL, 0, sect->index);
  ret = elfsh_insert_symbol(file->secthash[ELFSH_SECTION_SYMTAB],
			    &new, sect->name);
  if (ret < 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
  ret = elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]);

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Inserted STT_SECT symbol %s [" AFMT "] sz:%u cur:%u \n",
	 sect->name, (eresi_Addr) sect->shdr->sh_addr, 
	 sect->shdr->sh_size, sect->curend);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}


/**
 * Insert STT_SECTION symbol
 * @param file
 * @param name
 * @param vaddr
 * @param sz
 * @param sctidx
 * @return
 */
int		elfsh_insert_funcsym(elfshobj_t *file, char *name, 
				     eresi_Addr vaddr, 
				     uint32_t sz, uint32_t sctidx)
{
  elfsh_Sym	new;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  new = elfsh_create_symbol(vaddr, sz, STT_FUNC, STB_LOCAL, 0, sctidx);
  ret = elfsh_insert_symbol(file->secthash[ELFSH_SECTION_SYMTAB],
			    &new, name);
  if (ret < 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
  ret = elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]);

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Inserted STT_FUNC symbol %s [" AFMT 
	 "] sz:%u sctidx:%u \n",
	 name, vaddr, sz, sctidx);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}
