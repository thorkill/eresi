/*
** sym_common.c for elfsh
** 
** Started on  Tue Dec 31 10:19:01 2002 mayhem
** Last update Thu Jun 26 06:30:34 2003 mayhem
*/
#include "libelfsh.h"


/* Return the symbol size */
Elf32_Word	elfsh_get_symbol_size(Elf32_Sym *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh:get_symbol_size] Invalid NULL parameter\n", -1);
  return (s->st_size);
}

/* Change the symbol size */
int	elfsh_set_symbol_size(Elf32_Sym *s, Elf32_Word size)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh:set_symbol_size] Invalid NULL parameter\n", -1);
  s->st_size = size;
  return (0);
}

/* Return the symbo offset giving its Elf32_Sym entry */
Elf32_Addr	elfsh_get_symbol_value(Elf32_Sym *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  return (s->st_value);
}

/* Change the symbol offset giving its Elf32_Sym entry */
int	elfsh_set_symbol_value(Elf32_Sym *s, Elf32_Addr value)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  s->st_value = value;
  return (0);
}


/* Return the symbol binding */
u_char	elfsh_get_symbol_bind(Elf32_Sym *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  return (ELF32_ST_BIND(s->st_info));
}

/* Change the symbol binding */
u_char	elfsh_set_symbol_bind(Elf32_Sym *s, char opt)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  s->st_info = (s->st_info & 0x0F) + ((opt << 4) & 0xF0);
  return (0);
}

/* Return the symbol type */
u_char	elfsh_get_symbol_type(Elf32_Sym *s)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  return (ELF32_ST_TYPE(s->st_info));
}

/* Change the symbol type */
u_char	elfsh_set_symbol_type(Elf32_Sym *s, char type)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  s->st_info = (s->st_info & 0xF0) + (type & 0x0F);
  return (0);
}

/* Return the symbol type */
u_char	elfsh_get_symbol_visibility(Elf32_Sym *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  return (ELF32_ST_VISIBILITY(s->st_other));
}

/* Change the symbol type */
u_int	elfsh_set_symbol_visibility(Elf32_Sym *s, char vis)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  s->st_other = ELF32_ST_VISIBILITY(vis);
  return (0);
}

/* Return the symbol linked section index */
u_int	elfsh_get_symbol_link(Elf32_Sym *s)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  return (s->st_shndx);
}

/* Change the symbol linked section index */
u_int	elfsh_set_symbol_link(Elf32_Sym *s, u_int val)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  s->st_shndx = val;
  return (0);
}



/* High level procedure for get_sym_by_value() */
Elf32_Sym	*elfsh_get_metasym_byval(elfshobj_t *file, int num, 
					 u_int vaddr, int *off, int mode)
{
  Elf32_Sym	*res;

  elfsh_get_symtab(file, NULL);
  elfsh_get_dynsymtab(file, NULL);
  res = elfsh_get_sym_by_value(file->secthash[ELFSH_SECTION_DYNSYM]->data, 
			       num, vaddr, off, mode);
  if (res == NULL)
    res = elfsh_get_sym_by_value(file->secthash[ELFSH_SECTION_SYMTAB]->data, 
				 num, vaddr, off, mode);
  return (res);
}


/* Internal generic function for symbol research by value */
Elf32_Sym	  *elfsh_get_sym_by_value(Elf32_Sym *sym, int num, 
					  u_int vaddr, int *off, int mode)
{
  Elf32_Sym	  *low;
  Elf32_Sym	  *high;
  Elf32_Sym	  *good;
  int		  low_off;
  int		  high_off;
  int		  idx;
  
  low_off = high_off = ELFSH_UNSET_OFFSET;
  good = low = high = NULL;

  if (sym == NULL || !num)
    ELFSH_SETERROR("[libelfsh:get_sym_by_value] Invalid NULL parameter\n", NULL);

  /* main loop */
  for (idx = 0; idx < num; idx++)
    if (sym[idx].st_value < vaddr && (vaddr - sym[idx].st_value) < low_off)
      {
	low = sym + idx;
	low_off = vaddr - sym[idx].st_value;
      }
    else if (sym[idx].st_value > vaddr && (sym[idx].st_value - vaddr < high_off))
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
      return (good);
    }
  else if (mode == ELFSH_LOWSYM)
    {
      if (off != NULL)
	*off = low_off;
      return (low);
    }
  else if (mode == ELFSH_HIGHSYM)
    {
      if (off != NULL)
	*off = high_off;
      return (high);
    }

  /* ELFSH_EXACTSYM but no matching symbol */
  return (NULL);
}


/* Create a symbol */
Elf32_Sym	elfsh_create_symbol(int value, int size, int type, int binding, int vis, int sctidx)
{
  Elf32_Sym	new;

  new.st_value = value;
  new.st_size  = size;
  new.st_info  = ELF32_ST_INFO(binding, type);
  new.st_other = vis;
  new.st_shndx = sctidx;
  return (new);
}


/* Just copy the symtab . The returned symbol table need to be freed */
Elf32_Sym	*elfsh_copy_symtab(void *addr, int size)
{
  Elf32_Sym	*new;
  
  if (!addr || !size)
    ELFSH_SETERROR("[libelfsh:copy_symtab] Invalid NULL parameter\n", NULL);
  XALLOC(new, size * ELFSH_SYMTAB_ENTRY_SIZE, NULL);
  memcpy(new, addr, size * ELFSH_SYMTAB_ENTRY_SIZE);
  return (new);
}



/* Get symbol by index */
Elf32_Sym		*elfsh_get_symbol_by_index(Elf32_Sym *symtab, u_int index)
{
  return (symtab + index);
}


/* Get a symbol value by its name */
Elf32_Sym	*elfsh_get_metasym_by_name(elfshobj_t *file, char *name)
{
  Elf32_Sym	*sym;

  sym = elfsh_get_dynsymbol_by_name(file, name);
  if (sym != NULL)
    return (sym);
  sym = elfsh_get_symbol_by_name(file, name);
  return (sym);
}

/* Reverse a symbol */
char		*elfsh_reverse_metasym(elfshobj_t *file, u_int vaddr, int *off)
{
  elfshsect_t	*parent;

  if (file == NULL || vaddr == NULL)
    ELFSH_SETERROR("[libelfsh:reverse_metasmy] Invalid NULL parameter\n", 
		   NULL);
  parent = elfsh_get_parent_section(file, vaddr, NULL);
  if (parent == NULL)
    return (NULL);
  if (file->secthash[ELFSH_SECTION_PLT] != NULL &&
      parent->index == file->secthash[ELFSH_SECTION_PLT]->index)
    return (elfsh_reverse_dynsymbol(file, vaddr, off));
  return (elfsh_reverse_symbol(file, vaddr, off));
}



