/*
** reloc.c for elfsh
**
** Started on  Tue Feb 27 14:34:50 2001 mayhem
** Last update Thu Aug 21 03:28:41 2003 mayhem
*/
#include "libelfsh.h"


/* Return the relocation type */
u_int	elfsh_get_reltype(Elf32_Rel *r)
{
  if (!r)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  return (ELF32_R_TYPE(r->r_info));
}

/* Return the relocation type */
u_int	elfsh_set_reltype(Elf32_Rel *r, Elf32_Word type)
{
  if (!r)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  r->r_info = (r->r_info & 0xFF00) + ELF32_R_TYPE(type);
  return (0);
}

/* Return the relocation symbol index */
u_int	elfsh_get_relsym(Elf32_Rel *r)
{
  if (!r)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  return (ELF32_R_SYM(r->r_info));
}

/* Change the relocation symbol index */
u_int	elfsh_set_relsym(Elf32_Rel *r, Elf32_Word sym)
{
  if (!r)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  r->r_info = (r->r_info & 0x00FF) + ((sym << 8) & 0xFF00);
  return (0);
}


/* Return the relocation offset */
Elf32_Addr	elfsh_get_reloffset(Elf32_Rel *r)
{
  if (!r)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  return (r->r_offset);
}

/* Change the relocation offset for this entry */
int		elfsh_set_reloffset(Elf32_Rel *r, Elf32_Addr off)
{
  if (!r)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  r->r_offset = off;
  return (0);
}


/* Return the add-end */
Elf32_Sword     elfsh_get_reladdend(Elf32_Rela *r)
{
   if (r == NULL)
     ELFSH_SETERROR("[libelfsh:get_reladdend] Invalid NULL parameter\n", -1);
   return (r->r_addend);
}

/* Change the add-end */
int     elfsh_set_reladdend(Elf32_Rela *r, Elf32_Sword val)
{
   if (r == NULL)
     ELFSH_SETERROR("[libelfsh:set_reladdend] Invalid NULL parameter\n", -1);
   r->r_addend = val;
   return (0);
}




/* Return the 'range'th relocation table */
elfshsect_t	*elfsh_get_reloc(elfshobj_t *file, u_int range, u_int *nbr)
{
  elfshsect_t	*s;
  u_int		type;

  /* Sanity checks */
  if (file->sectlist == NULL && elfsh_get_sht(file, NULL) == NULL)
    return (NULL);

  /* Read section */
  type = (IS_REL(file->sectlist) ? SHT_REL : SHT_RELA);
  s = elfsh_get_section_by_type(file, type,
				range, NULL,
				NULL, NULL);
  if (s == NULL)
    return (NULL);

  /* Load section data */
  if (s->data == NULL)
    {
      s->data = elfsh_load_section(file, s->shdr);
      if (s->data == NULL)
	return (NULL);
    }

  /* Return what's needed */
  if (nbr != NULL)
     *nbr = s->shdr->sh_size / (s->shdr->sh_type == SHT_REL ? 
				sizeof(Elf32_Rel) : sizeof(Elf32_Rela));;
  return (s);
}




/* Insert a symbol in the given symbol table */
int		elfsh_insert_relent(elfshsect_t *sect, Elf32_Rel *rel)

{
  int		index;

  /* Sanity checks */
  if (sect == NULL || sect->shdr == NULL || rel == NULL)
    ELFSH_SETERROR("[libelfsh:insert_relent] Invalid NULL paramater\n", -1);
  if (sect->shdr->sh_type != SHT_REL && sect->shdr->sh_type != SHT_RELA)
    ELFSH_SETERROR("[libelfsh:insert_symbol] Input section is not REL/RELA\n",
		   -1);

  /* Insert entry in relocation table */
   if (sect->shdr->sh_type == SHT_REL)
     index = elfsh_append_data_to_section(sect, rel, sizeof(Elf32_Rel));
   else
     index = elfsh_append_data_to_section(sect, rel, sizeof(Elf32_Rela));
  return (index);
}



/* Return the symbol name associated with the relocation entry */
char		*elfsh_get_symname_from_reloc(elfshobj_t *file,
					      Elf32_Rel	 *r)
{
  Elf32_Sym	*sym;

  /* Sanitize */
  sym = elfsh_get_symbol_from_reloc(file, r);
  if (sym == NULL)
    return (NULL);

  /* The object is relocatable : find symbol in .symtab */
  if (file->hdr->e_type == ET_REL)
    return (elfsh_get_symbol_name(file, sym));

  /* else find symbol in .dynsym */
  return (elfsh_get_dynsymbol_name(file, sym));
}



/* Return the Symbol associated with the relocation entry */
Elf32_Sym	*elfsh_get_symbol_from_reloc(elfshobj_t *file,
					     Elf32_Rel	*r)
{
  Elf32_Sym	*sym;
  u_int		tmp;

  /* Sanity checks */
  if (NULL == file || NULL == r)
    ELFSH_SETERROR("[libelfsh:get_symbol_from_reloc] Invalid NULL parameter\n",
		   NULL);
  tmp = ELF32_R_SYM(r->r_info);

  /* The object is relocatable : find symbol in .symtab */
  if (file->hdr->e_type == ET_REL)
    {
      if (NULL == file->secthash[ELFSH_SECTION_SYMTAB])
	if (NULL == elfsh_get_symtab(file, NULL))
	  return (NULL);
      sym = file->secthash[ELFSH_SECTION_SYMTAB]->data + 
	(tmp * ELFSH_SYMTAB_ENTRY_SIZE);
      return (sym);
    }

  /* else find symbol in .dynsym */
  if (NULL == file->secthash[ELFSH_SECTION_DYNSYM] &&
      NULL == elfsh_get_dynsymtab(file, NULL))
    return (NULL);
  sym = file->secthash[ELFSH_SECTION_DYNSYM]->data + 
    (tmp * ELFSH_SYMTAB_ENTRY_SIZE);
  return (sym);
}



/* Used as internal handler for elfsh hashes */
Elf32_Rel	*elfsh_get_relent_by_index(Elf32_Rel *table, u_int index)
{
  if (!table)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", NULL);
  return (table + index);
}



/* Used as internal handler for elfsh hashes */
Elf32_Rel	*elfsh_get_relent_by_name(elfshobj_t *file, char *name)
{
  elfshsect_t	*sect;
  Elf32_Rel	*cur;
  u_int		range;
  u_int		idx;
  u_int		num;
  char		*curnam;

  /* Sanity checks */
  if (file == NULL || name == NULL)
    ELFSH_SETERROR("[libelfsh:get_relent_by_name] Invalid NULL parameter\n",
		   NULL);

  /* Find relocation entry by name */
  sect = elfsh_get_reloc(file, 0, &num);
  for (range = 1; NULL != sect; range++)
    {
      for (idx = 0; idx < num; idx++)
	{
	  cur = (sect->shdr->sh_type == SHT_RELA ?
		 (void *) ((Elf32_Rela *) sect->data + idx) :
		 (void *) ((Elf32_Rel  *) sect->data + idx));
	  curnam = elfsh_get_symname_from_reloc(file, cur);
	  if (curnam != NULL && !strcmp(curnam, name))
	    return (cur);
	}
      sect = elfsh_get_reloc(file, range, &num);
    }

  /* Not found */
  ELFSH_SETERROR("[libelfsh:get_relent_by_name] Relentry not found\n", NULL);
}



/* Create relocation table for section */
elfshrel_t	*elfsh_find_rel(elfshsect_t *sect)
{
  elfshsect_t	*target;
  elfshrel_t	*rel;
  int		*dword;
  char		*str;
  u_int		vaddr;
  u_int		index;

  /* Sanity checks */
  if (sect == NULL)
    ELFSH_SETERROR("[libelfsh:find_rel] Invalid NULL parameter\n", NULL);
  else if (sect->data == NULL)
    ELFSH_SETERROR("[libelfsh:find_rel] Section empty\n", NULL);
  else if (sect->shdr->sh_addr == NULL)
    ELFSH_SETERROR("[libelfsh:find_rel] Section unmapped\n", NULL);
  else if (sect->rel)
    return (sect->rel);

  /* These sections must not be relocated, but passed to relative */
  if (sect->shdr->sh_type == SHT_DYNSYM             ||
      sect->shdr->sh_type == SHT_REL                ||
      sect->shdr->sh_type == SHT_RELA               ||
      sect->shdr->sh_type == SHT_DYNAMIC            ||
      !strcmp(sect->name, ELFSH_SECTION_NAME_GOT)   ||
      !strcmp(sect->name, ELFSH_SECTION_NAME_CTORS) ||
      !strcmp(sect->name, ELFSH_SECTION_NAME_DTORS))
    return (NULL);

  /* Free existing rel[] entries if existing (section data probably changed) */
  sect->srcref = sect->dstref = 0;

  /* Read the actual section and find valid references */
  str = sect->data;
  for (dword = (int *) str;
       ((char *) dword + 4) <= ((char *) sect->data + sect->shdr->sh_size);
       dword = (int *) str)
    {
      target = elfsh_get_parent_section(sect->parent, *dword, NULL);
      if (target != NULL)
	{
	  sect->srcref++;
	  target->dstref++;
	}
      str++;
    }

  /* allocate the array */
  if (sect->srcref == 0)
    ELFSH_SETERROR("[libelfsh:find_rel] No need to relocate section\n", NULL);
  XALLOC(rel, sect->srcref * sizeof(elfshrel_t), NULL);

  /* Read the actual section again and create section rel table */
  str = sect->data;
  for (index = 0, dword = (int *) str;
       ((char *) dword + 4) <= ((char *) sect->data + sect->shdr->sh_size);
       dword = (int *) str)
    {
      target = elfsh_get_parent_section(sect->parent, *dword, NULL);
      if (target != NULL)
	{
	  vaddr = sect->shdr->sh_addr + (u_int) str - (u_int) sect->data;
	  rel[index].idx_src = sect->index;
	  rel[index].off_src = vaddr - sect->shdr->sh_addr;
	  rel[index].idx_dst = target->index;
	  rel[index].off_dst = *dword - target->shdr->sh_addr;
	  rel[index].type    = ELFSH_RELOC_SECTBASE;
	  index++;
	}
      str++;
    }

  /* Return the array */
  sect->rel = rel;
  return (sect->rel);
}




