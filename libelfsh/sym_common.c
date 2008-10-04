/**
 * @file sym_common.c
 * @ingroup libelfsh
 * sym_common.c for elfsh
 * 
 * Started on  Tue Dec 31 10:19:01 2002 jfv
 * 
 *
 * $Id: sym_common.c,v 1.13 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libelfsh.h"

/**
 * Return the symbol size 
 * @param s Symbol
 * @return
 */
elfsh_Word	elfsh_get_symbol_size(elfsh_Sym *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->st_size));
}

/**
 * Change the symbol size 
 * @param s Symbol
 * @param size New size of symbol
 * @return 0 on success or -1 on error.
 */
int	elfsh_set_symbol_size(elfsh_Sym *s, eresi_Addr size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  s->st_size = (elfsh_Word) size;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the symbol offset giving its elfsh_Sym entry
 * @param s
 * @return
 */
eresi_Addr	elfsh_get_symbol_value(elfsh_Sym *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->st_value));
}

/**
 * Change the symbol offset giving its elfsh_Sym entry
 * @param s
 * @param value
 * @return
 */
int	elfsh_set_symbol_value(elfsh_Sym *s, eresi_Addr value)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  s->st_value = value;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Return the symbol binding
 * @param s
 * @return
 */
unsigned char	elfsh_get_symbol_bind(elfsh_Sym *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ST_BIND(s->st_info)));
}

/**
 * Change the symbol binding
 * @param s
 * @param opt
 * @return
 */
unsigned char	elfsh_set_symbol_bind(elfsh_Sym *s, eresi_Addr opt)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  s->st_info = (s->st_info & 0x0F) + ((((char) opt) << 4) & 0xF0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the symbol type
 * @param s
 * @return
 */
unsigned char	elfsh_get_symbol_type(elfsh_Sym *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ST_TYPE(s->st_info)));
}

/**
 * Change the symbol type 
 * @param s
 * @param type
 * @return
 */
unsigned char	elfsh_set_symbol_type(elfsh_Sym *s, eresi_Addr type)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  s->st_info = (s->st_info & 0xF0) + (((char) type) & 0x0F);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the symbol type
 * @param s
 * @return
 */
unsigned char	elfsh_get_symbol_visibility(elfsh_Sym *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ST_VISIBILITY(s->st_other)));
}

/**
 * Change the symbol type
 * @param s
 * @param vis
 * @return
 * 
 */
unsigned int	elfsh_set_symbol_visibility(elfsh_Sym *s, eresi_Addr vis)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  s->st_other = ELFSH_ST_VISIBILITY(((char) vis));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the symbol linked section index
 * @param s
 * @return
 */
unsigned int	elfsh_get_symbol_link(elfsh_Sym *s) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->st_shndx));
}

/**
 * Change the symbol linked section index
 * @param s
 * @param val
 * @return
 */
unsigned int	elfsh_set_symbol_link(elfsh_Sym *s, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  s->st_shndx = (unsigned int) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * High level procedure for get_sym_by_value() 
 * @param file
 * @param vaddr
 * @param off
 * @param mode
 * @return
 */
elfsh_Sym	*elfsh_get_metasym_by_value(elfshobj_t *file, 
					    eresi_Addr vaddr, 
					    int *off, int mode)
{
  elfsh_Sym	*dynsym;
  elfsh_Sym	*symtab;
  elfsh_Sym	*res;
  int		num;
  int		dynum;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  symtab = elfsh_get_symtab(file, &num);
  dynsym = elfsh_get_dynsymtab(file, &dynum);

  /* ET_DYN objects have a relative addressing inside the ELF file */
  if (elfsh_is_debug_mode())
    vaddr -= file->rhdr.base;

  res = elfsh_get_sym_by_value(dynsym, dynum, vaddr, off, mode);
  if (res == NULL)
    res = elfsh_get_sym_by_value(symtab, num, vaddr, off, mode);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (res));
}


/**
 * Internal generic function for symbol research by value
 * @param sym
 * @param num
 * @param vaddr
 * @param off
 * @param mode
 * @return
 */
elfsh_Sym	  *elfsh_get_sym_by_value(elfsh_Sym *sym, int num, 
					  eresi_Addr vaddr, int *off, int mode)
{
  elfsh_Sym	  *low;
  elfsh_Sym	  *high;
  elfsh_Sym	  *good;
  int		  low_off;
  int		  high_off;
  int		  idx;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  low_off = high_off = ELFSH_UNSET_OFFSET;
  good = low = high = NULL;

  if (sym == NULL || !num)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* main loop */
  for (idx = 0; idx < num; idx++)
    if (sym[idx].st_value < vaddr && (vaddr - sym[idx].st_value) < low_off)
      {
	low = sym + idx;
	low_off = vaddr - sym[idx].st_value;
      }
    else if (sym[idx].st_value > vaddr &&
	     (sym[idx].st_value - vaddr < high_off))
      {
	high = sym + idx;
	high_off = sym[idx].st_value - vaddr;
      }
    else if (sym[idx].st_value == vaddr)
      good = sym + idx;

  /* Return what was asked */
  if (good != NULL)
    {
      if (off != NULL)
	*off = 0;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (good));
    }
  else if (mode == ELFSH_LOWSYM)
    {
      if (off != NULL)
	*off = low_off;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (low));
    }
  else if (mode == ELFSH_HIGHSYM)
    {
      if (off != NULL)
	*off = high_off;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (high));
    }

  /* ELFSH_EXACTSYM but no matching symbol */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "No exact symbol matching", NULL);
}


/**
 * Create a symbol
 * @param value
 * @param size
 * @param type
 * @param binding
 * @param vis
 * @param sctidx
 * @return
 */
elfsh_Sym	elfsh_create_symbol(eresi_Addr value, int size, int type, 
				    int binding, int vis, int sctidx)
{
  elfsh_Sym	enew;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  enew.st_value = value;
  enew.st_size  = size;
  enew.st_info  = ELFSH_ST_INFO(binding, type);
  enew.st_other = vis;
  enew.st_shndx = sctidx;
	enew.st_name  = NULL;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (enew));
}


/**
 * Just copy the symtab . The returned symbol table need to be freed.
 * @param data
 * @param size
 * @return
 */
elfsh_Sym	*elfsh_copy_symtab(void *data, int size)
{
  elfsh_Sym	*enew;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!data || !size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,enew, size * ELFSH_SYMTAB_ENTRY_SIZE, NULL);
  memcpy(enew, data, size * ELFSH_SYMTAB_ENTRY_SIZE);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (enew));
}

/**
 * Get symbol by index
 * @param symtab
 * @param index
 * @return
 */
elfsh_Sym		*elfsh_get_symbol_by_index(elfsh_Sym *symtab, 
						   eresi_Addr index)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (symtab + index));
}


/**
 * Get a symbol value by its name 
 * @param file
 * @param name
 * @return
 */
elfsh_Sym	*elfsh_get_metasym_by_name(elfshobj_t *file, char *name)
{
  elfsh_Sym	*sym;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  sym = elfsh_get_dynsymbol_by_name(file, name);
  if (sym != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym));
  sym = elfsh_get_symbol_by_name(file, name);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym));
}

/**
 * Reverse a symbol and give the offset of virtual address from that
 * symbol virtual address.
 * @param file elfsh object
 * @param vaddr Virtual address to resolve
 * @param off Reference to store offset from the symbol address
 * @return Returns a pointer to the resolved symbol.
 */
char		*elfsh_reverse_metasym(elfshobj_t *file, 
				       eresi_Addr vaddr, elfsh_SAddr *off)
{
  elfshsect_t	*parent;
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || vaddr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);

  parent = elfsh_get_parent_section(file, vaddr, NULL);
  if (parent == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find parent section", NULL);

  if ((file->secthash[ELFSH_SECTION_PLT] != NULL &&
       parent->index == file->secthash[ELFSH_SECTION_PLT]->index) ||
      (file->secthash[ELFSH_SECTION_INIT] != NULL && 
       parent->index == file->secthash[ELFSH_SECTION_INIT]->index) ||
      (file->secthash[ELFSH_SECTION_FINI] != NULL && 
       parent->index == file->secthash[ELFSH_SECTION_FINI]->index))
    {
      name = elfsh_reverse_dynsymbol(file, vaddr, off);
      if (name)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, name);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_reverse_symbol(file, vaddr, off)));
}

/**
 * Endianize the table 
 * @param tab
 * @return
 */
int		elfsh_endianize_symtab(elfshsect_t *tab)
{
  elfsh_Sym	*symtab;
  unsigned int		idx;
  unsigned int		sz;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!tab)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);

    /* Arrange endianess if necessary */
#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (tab->parent->hdr->e_ident[EI_DATA] == ELFDATA2MSB) 
    {
#elif __BYTE_ORDER == __BIG_ENDIAN
  if (tab->parent->hdr->e_ident[EI_DATA] == ELFDATA2LSB) 
    {
#else
#error Unexpected __BYTE_ORDER !
#endif
      symtab = tab->data;
      sz = tab->curend / sizeof(elfsh_Sym);
      for (idx = 0; idx < sz; idx++)
	{
	  symtab[idx].st_name  = swap32(symtab[idx].st_name);
	  symtab[idx].st_shndx = swap16(symtab[idx].st_shndx);
	  symtab[idx].st_value = swaplong(symtab[idx].st_value);
	  symtab[idx].st_size  = swaplong(symtab[idx].st_size);
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
  

/**
 * Shift the symbol table
 * @param file
 * @param symtab
 * @param limit
 * @param inc
 * @return
 */
  int		elfsh_shift_syms(elfshobj_t *file, 
				 elfshsect_t *symtab, 
				 eresi_Addr limit, int inc)
{
#if	__DEBUG_RELADD__
  static unsigned int	totshift = 0;
#endif
  unsigned int		nbr;
  unsigned int		idx;
  elfsh_Sym	*sym;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (symtab == NULL || symtab->data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid SYMTAB parameter", -1);

#if	__DEBUG_RELADD__
  totshift += inc;
  printf("[DEBUG_RELADD] Shifting symtab [LIM " AFMT 
	 " .::. INC %08u .::. TOT %08u] \n",
	 (eresi_Addr) limit, inc, totshift);
#endif


  sym = (elfsh_Sym *) symtab->data;
  nbr = symtab->shdr->sh_size / ELFSH_SYMTAB_ENTRY_SIZE;
  for (idx = 0; idx < nbr; idx++)
    if (sym[idx].st_value >= limit && limit != ELFSH_NOLIMIT)
      {

#if __DEBUG_RELADD__
	printf("[DEBUG_RELADD] Shifted symbol *%-20s* [" AFMT "-> " AFMT "]\n",
	       elfsh_get_symbol_name(file, sym),
	       (eresi_Addr) sym[idx].st_value,
	       (eresi_Addr) sym[idx].st_value + inc);
#endif

	sym[idx].st_value += inc;
      }

  elfsh_sync_sorted_symtab(symtab);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * If submited function point into the plt, we return the file
 * and its virtual address to point to remote function
 * @param filein base file pointer
 * @param vaddrin base virtual address
 * @param fileout returned file pointer
 * @param vaddrout returned virtual address
 * @return
 */
int			elfsh_resolv_remote_function(elfshobj_t *filein, eresi_Addr vaddrin,
						     elfshobj_t **fileout, eresi_Addr *vaddrout)
{
  elfshobj_t		*file;
  elfshsect_t		*sect;
  char			*data;
  elfsh_SAddr		sfoffset;
  char			*sym_name;
  elfsh_Sym		*sym;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!filein || !fileout || !vaddrout)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Default returned values are -in */
  *fileout = filein;
  *vaddrout = vaddrin;

  /* Get virtual address parent section */
  sect = elfsh_get_parent_section(filein, vaddrin, &sfoffset);

  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't get section", -1);

  data = elfsh_get_section_name(filein, sect);

  if (!data)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't get section name", -1);

  /* Find the right function on dependencies */
  if (!strncmp(data, ".plt", 4))
    {
      sym_name = elfsh_reverse_dynsymbol(filein, vaddrin, &sfoffset);

      if (!sym_name)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Can't find symbol name", -1);

      file = elfsh_symbol_search(filein, sym_name);
	      
      if (!file)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Can't find extern function file", -1);

      sym = elfsh_get_dynsymbol_by_name(file, sym_name);

      if (!sym)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Can't find function symbol on dependencies", -1);

      /* Update pointers */
      *fileout = file;
      *vaddrout = sym->st_value;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Search a symbol on dependences file 
 * @param file parent file
 * @param name symbol name
 * @return file object that containt the symbol (on dynsym)
 */
elfshobj_t   		*elfsh_symbol_search(elfshobj_t *file, char *name)
{
  char			**keys;
  int			keynbr;
  unsigned int			index;
  elfshobj_t		*child, *find = NULL;
  elfsh_Sym		*sym;
  elfshsect_t		*sect;
  char			*sect_name;
  unsigned int			bindnum;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 
  
  keys = hash_get_keys(&(file->child_hash), &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  child = (elfshobj_t *) hash_get(&(file->child_hash), keys[index]);

	  if (child)
	    {
	      sym = elfsh_get_dynsymbol_by_name(child, name);

	      if (!sym)
		continue;

	      bindnum = elfsh_get_symbol_bind(sym);
	      
	      /* We need a global or weak symbol */
	      if (bindnum == STB_GLOBAL || bindnum == STB_WEAK)
		{
		  sect = elfsh_get_section_by_index(child, sym->st_shndx, NULL, NULL);

		  /* The symbol must be local and pointing on sect_name */
		  if (sect && (sect_name = elfsh_get_section_name(child, sect)) != NULL
		      && !strncmp(sect_name, ".text", 5))
		    {
		      find = child;
		      break;
		    }
		}
	    }
	  
	  /* Search deeper on this child */
	  find = elfsh_symbol_search(child, name);
	  
	  if (find)
	    break;
	}
    }
  
  hash_free_keys(keys);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, find);
}
