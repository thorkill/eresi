/**
 * @file sym_common.c
** sym_common.c for elfsh
** 
** Started on  Tue Dec 31 10:19:01 2002 mayhem
** 
**
** $Id: sym_common.c,v 1.8 2007-06-27 11:25:12 heroine Exp $
**
*/
#include "libelfsh.h"

/**
 * Return the symbol size 
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
 */
int	elfsh_set_symbol_size(elfsh_Sym *s, elfsh_Addr size)
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
 */
elfsh_Addr	elfsh_get_symbol_value(elfsh_Sym *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->st_value));
}

/**
 * Change the symbol offset giving its elfsh_Sym entry 
 */
int	elfsh_set_symbol_value(elfsh_Sym *s, elfsh_Addr value)
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
 */
u_char	elfsh_get_symbol_bind(elfsh_Sym *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ST_BIND(s->st_info)));
}

/**
 * Change the symbol binding 
 */
u_char	elfsh_set_symbol_bind(elfsh_Sym *s, elfsh_Addr opt)
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
 */
u_char	elfsh_get_symbol_type(elfsh_Sym *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ST_TYPE(s->st_info)));
}

/**
 * Change the symbol type 
 */
u_char	elfsh_set_symbol_type(elfsh_Sym *s, elfsh_Addr type)
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
 */
u_char	elfsh_get_symbol_visibility(elfsh_Sym *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ST_VISIBILITY(s->st_other)));
}

/**
 * Change the symbol type 
 */
u_int	elfsh_set_symbol_visibility(elfsh_Sym *s, elfsh_Addr vis)
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
 */
u_int	elfsh_get_symbol_link(elfsh_Sym *s) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->st_shndx));
}

/**
 * Change the symbol linked section index 
 */
u_int	elfsh_set_symbol_link(elfsh_Sym *s, elfsh_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  s->st_shndx = (u_int) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * High level procedure for get_sym_by_value() 
 */
elfsh_Sym	*elfsh_get_metasym_by_value(elfshobj_t *file, elfsh_Addr vaddr, 
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
 */
elfsh_Sym	  *elfsh_get_sym_by_value(elfsh_Sym *sym, int num, 
					  elfsh_Addr vaddr, int *off, int mode)
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
 */
elfsh_Sym	elfsh_create_symbol(elfsh_Addr value, int size, int type, 
				    int binding, int vis, int sctidx)
{
  elfsh_Sym	new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  new.st_value = value;
  new.st_size  = size;
  new.st_info  = ELFSH_ST_INFO(binding, type);
  new.st_other = vis;
  new.st_shndx = sctidx;
	new.st_name  = NULL;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/**
 * Just copy the symtab . The returned symbol table need to be freed 
 */
elfsh_Sym	*elfsh_copy_symtab(void *data, int size)
{
  elfsh_Sym	*new;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!data || !size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, size * ELFSH_SYMTAB_ENTRY_SIZE, NULL);
  memcpy(new, data, size * ELFSH_SYMTAB_ENTRY_SIZE);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/**
 * Get symbol by index 
 */
elfsh_Sym		*elfsh_get_symbol_by_index(elfsh_Sym *symtab, 
						   elfsh_Addr index)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (symtab + index));
}


/**
 * Get a symbol value by its name 
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
				       elfsh_Addr vaddr, elfsh_SAddr *off)
{
  elfshsect_t	*parent;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || vaddr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);

  parent = elfsh_get_parent_section(file, vaddr, NULL);
  if (parent == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find parent section", NULL);

  if (file->secthash[ELFSH_SECTION_PLT] != NULL &&
      parent->index == file->secthash[ELFSH_SECTION_PLT]->index)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       (elfsh_reverse_dynsymbol(file, vaddr, off)));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_reverse_symbol(file, vaddr, off)));
}

/**
 * Endianize the table 
 */
int		elfsh_endianize_symtab(elfshsect_t *tab)
{
  elfsh_Sym	*symtab;
  u_int		idx;
  u_int		sz;

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
 */
  int		elfsh_shift_syms(elfshobj_t *file, 
				 elfshsect_t *symtab, 
				 elfsh_Addr limit, int inc)
{
#if	__DEBUG_RELADD__
  static u_int	totshift = 0;
#endif
  u_int		nbr;
  u_int		idx;
  elfsh_Sym	*sym;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (symtab == NULL || symtab->data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid SYMTAB parameter", -1);

#if	__DEBUG_RELADD__
  totshift += inc;
  printf("[DEBUG_RELADD] Shifting symtab [LIM " AFMT 
	 " .::. INC %08u .::. TOT %08u] \n",
	 (elfsh_Addr) limit, inc, totshift);
#endif


  sym = (elfsh_Sym *) symtab->data;
  nbr = symtab->shdr->sh_size / ELFSH_SYMTAB_ENTRY_SIZE;
  for (idx = 0; idx < nbr; idx++)
    if (sym[idx].st_value >= limit && limit != ELFSH_NOLIMIT)
      {

#if __DEBUG_RELADD__
	printf("[DEBUG_RELADD] Shifted symbol *%-20s* [" AFMT "-> " AFMT "]\n",
	       elfsh_get_symbol_name(file, sym),
	       (elfsh_Addr) sym[idx].st_value,
	       (elfsh_Addr) sym[idx].st_value + inc);
#endif

	sym[idx].st_value += inc;
      }

  elfsh_sync_sorted_symtab(symtab);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
