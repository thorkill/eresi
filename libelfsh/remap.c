/*
** remap.c for elfsh
**
** elfsh_reloc_pht/sht/symtab() are based on spacewalker's 
** original modremap.c
** 
** Started on  Mon Mar 17 09:30:33 2003 mayhem
** Last update Tue Jun  3 14:01:26 2003 mayhem
*/
#include "libelfsh.h"


/* Remap the Program Header Table */
int		elfsh_reloc_pht(elfshobj_t *file, u_long diff)
{
  u_int		i;
  u_int		count;
  u_long	base;

  base = elfsh_get_object_baseaddr(file);
  if (file == NULL || file->pht == NULL || file->hdr->e_phnum == 0)
    ELFSH_SETERROR("[libelfsh:reloc_pht] Invalid NULL parameter\n", 0);
  for (count = i = 0; i < file->hdr->e_phnum; i++) 
    {
      if (file->pht[i].p_vaddr >= base)
	{
	  file->pht[i].p_vaddr += diff;
	  count++;
	}
      if (file->pht[i].p_paddr >= base)
	{
	  file->pht[i].p_paddr += diff;
	  count++;
	}
    }
  return (count);
}

/* Remap the Section Header Table */
int		elfsh_reloc_sht(elfshobj_t *file, u_long diff)
{
  u_int		i;
  u_int		count;
  u_long	base;

  if (file == NULL || file->sht == NULL || file->hdr->e_shnum == 0)
    ELFSH_SETERROR("[libelfsh:reloc_sht] Invalid NULL parameter\n", -1);
  base = elfsh_get_object_baseaddr(file);
  for (count = i = 0; i < file->hdr->e_shnum; i++)
    if (file->sht[i].sh_addr > base)
      {
	file->sht[i].sh_addr += diff;
	count++;
      }
  return (count);
}



/* Remap a section which type is SHT_SYMTAB or SHT_DYNSYM */
int		elfsh_reloc_symtab(elfshsect_t *s, u_long diff)
{
  Elf32_Sym	*symtab;
  u_int		i;
  u_int		vaddr;
  u_int		count;
  u_long	base;

  if (s == NULL || s->shdr == NULL)
    ELFSH_SETERROR("[libelfsh:reloc_symtab] Invalid NULL parameter\n", -1);
  else if (s->shdr->sh_type != SHT_SYMTAB && s->shdr->sh_type != SHT_DYNSYM)
    ELFSH_SETERROR("[libelfsh:reloc_symtab] Unexpected section type\n", -1);
  symtab = s->data;
  base = elfsh_get_object_baseaddr(s->parent);
  for (count = i = 0; i < s->shdr->sh_size / sizeof(Elf32_Sym); i++)
    {
      vaddr = elfsh_get_symbol_value(symtab + i);
      if (vaddr > base) 
	{
	  elfsh_set_symbol_value(symtab + i, vaddr + diff);
	  count++;
	}
    }
  
  /* Synchronize the symtab hash table */
  elfsh_sync_sorted_symtab(s);
  return (count);
}


/* Remap a section using its extra relocation entries */
int		elfsh_reloc_raw(elfshsect_t *cur, u_long diff)
{
  u_int		index;
  u_long	addr;
  elfshsect_t	*target;
  char		*str;

  if (cur == NULL || cur->shdr == NULL)
    ELFSH_SETERROR("[libelfsh:reloc_raw] Invalid NULL parameter\n", -1);
  if (cur->data == NULL || cur->rel == NULL)
    return (0);

  /* Read the actual section and find valid references */
  for (index = 0; index < cur->srcref; index++)
    switch (cur->rel[index].type)
      {

	/* Relocate by : section[idx_dst].vaddr + off_dst */
      case ELFSH_RELOC_SECTBASE:
	target = elfsh_get_section_by_index(cur->parent, 
					    cur->rel[index].idx_dst,
					    NULL, NULL);
	if (target == NULL)
	  ELFSH_SETERROR("[libelfsh:reloc_raw] Invalid IDX_DST\n", -1);
	str = cur->data + cur->rel[index].off_src;
	addr = target->shdr->sh_addr + cur->rel[index].off_dst + diff;
	memcpy(str, &addr, sizeof(u_long));

	/* Do not relocate */
      case ELFSH_RELOC_FP:
      default:
	break;
      }

  return (cur->srcref);
}



/* Remap the .dynamic section */
int		elfsh_reloc_dynamic(elfshsect_t *sect, u_long diff)
{
  elfshsect_t	*parent;
  Elf32_Dyn	*dyn;
  u_int		index;
  u_int		count;
  u_long	val;
  u_int		nbr;

  if (sect == NULL || sect->shdr == NULL)
    ELFSH_SETERROR("[libelfsh:reloc_rel] Invalid NULL parameter\n", -1);
  else if (sect->shdr->sh_type != SHT_DYNAMIC)
    ELFSH_SETERROR("[libelfsh:reloc_rel] Unexpected section type\n", -1);

  nbr = sect->shdr->sh_size / sizeof(Elf32_Dyn);
  for (dyn = sect->data, count = index = 0; index < nbr; index++)
    {
      val = elfsh_get_dynentry_val(dyn + index);
      parent = elfsh_get_parent_section(sect->parent, val, NULL);
      if (val && parent != NULL && parent->shdr->sh_addr != NULL)
	{
	  elfsh_set_dynentry_val(dyn + index, val + diff);
	  count++;
	}
    }
  return (count);
}


/* Remap sections of type SHT_REL and SHT_RELA */
int		elfsh_reloc_rel(elfshsect_t *sect, u_long diff)
{
  elfshsect_t	*parent;
  Elf32_Rel	*rel;
  u_int		index;
  u_int		count;
  u_int		nbr;

  if (sect == NULL || sect->shdr == NULL)
    ELFSH_SETERROR("[libelfsh:reloc_rel] Invalid NULL parameter\n", -1);
  else if (sect->shdr->sh_type != SHT_REL && sect->shdr->sh_type != SHT_RELA)
    ELFSH_SETERROR("[libelfsh:reloc_rel] Unexpected section type\n", -1);

  nbr = sect->shdr->sh_size / sizeof(Elf32_Rel);
  for (rel = sect->data, count = index = 0; index < nbr; index++)
    {
      parent = elfsh_get_parent_section(sect->parent, 
					rel[index].r_offset, 
					NULL);
      if (rel[index].r_offset && parent != NULL && parent->shdr->sh_addr != NULL)
	{
	  rel[index].r_offset += diff;
	  count++;
	}
    }
  return (count);
}



/* Remap section's whoose type is a data array (GOT, CTORS, DTORS ..) */
int		elfsh_reloc_array(elfshobj_t *file, u_long *array, u_int size, u_long diff)
{
  elfshsect_t	*parent;
  u_int		index;
  u_int		count;

  if (file == NULL || array == NULL)
    ELFSH_SETERROR("[libelfsh:reloc_array] Invalid NULL paramater\n", -1);
  for (count = index = 0; index < size; index++)
    {
      parent = elfsh_get_parent_section(file, array[index], NULL);
      if (parent != NULL && parent->shdr->sh_addr != NULL && array[index] != NULL)
	{  
	  array[index] += diff;
	  count++;
	}
    }
  return (count);
}


/* Remap Global Offset Table */
int		elfsh_reloc_got(elfshsect_t *sect, u_long diff)
{
  if (sect == NULL || sect->shdr == NULL)
    ELFSH_SETERROR("[libelfsh:reloc_got] Invalid NULL parameter\n", -1);
  else if (strcmp(sect->name, ELFSH_SECTION_NAME_GOT))
    ELFSH_SETERROR("[libelfsh:reloc_got] Unexpected section name\n", -1);
  return (elfsh_reloc_array(sect->parent, sect->data, 
			    sect->shdr->sh_size / sizeof(u_long), diff));
}

/* Remap .ctors section */
int		elfsh_reloc_ctors(elfshsect_t *sect, u_long diff)
{
  if (sect == NULL || sect->shdr == NULL)
    ELFSH_SETERROR("[libelfsh:reloc_ctors] Invalid NULL parameter\n", -1);
  else if (strcmp(sect->name, ELFSH_SECTION_NAME_CTORS))
    ELFSH_SETERROR("[libelfsh:reloc_ctors] Unexpected section name\n", -1);
  return (elfsh_reloc_array(sect->parent, sect->data, 
			    sect->shdr->sh_size / sizeof(u_long), diff));
}

/* Remap .dtors section */
int		elfsh_reloc_dtors(elfshsect_t *sect, u_long diff)
{
  if (sect == NULL || sect->shdr == NULL)
    ELFSH_SETERROR("[libelfsh:reloc_dtors] Invalid NULL parameter\n", -1);
  else if (strcmp(sect->name, ELFSH_SECTION_NAME_DTORS))
    ELFSH_SETERROR("[libelfsh:reloc_dtors] Unexpected section name\n", -1);
  return (elfsh_reloc_array(sect->parent, sect->data, 
			    sect->shdr->sh_size / sizeof(u_long), diff));
}


/* Not used ATM since it triggers more false positives ;P */
int		elfsh_reloc_hash(elfshsect_t *sect, u_long diff)
{
  if (sect == NULL || sect->shdr == NULL)
    ELFSH_SETERROR("[libelfsh:reloc_hash] Invalid NULL parameter\n", -1);
  else if (sect->shdr->sh_type != SHT_HASH)
    ELFSH_SETERROR("[libelfsh:reloc_hash] Unexpected section type\n", -1);
  return (elfsh_reloc_array(sect->parent, sect->data, 
			    sect->shdr->sh_size / sizeof(u_long), diff));
}


/* Call the type dependant remapping routine for this section */
int		elfsh_relocate_section(elfshsect_t *sect, u_long diff)
{
  int		ret;

  ret = 0;
  if (sect == NULL || sect->data == NULL)
    ELFSH_SETERROR("[libelfsh:reloc_section] Invalid NULL paramater\n", -1);
  if (sect->shdr->sh_addr != NULL)
    elfsh_find_rel(sect);

  if (sect->shdr->sh_type == SHT_SYMTAB)
    ret = elfsh_reloc_symtab(sect, diff);
  else if (sect->shdr->sh_type == SHT_DYNSYM)
    ret = elfsh_reloc_symtab(sect, diff);
  else if (sect->shdr->sh_type == SHT_RELA || sect->shdr->sh_type == SHT_REL)
    ret = elfsh_reloc_rel(sect, diff);
  else if (sect->shdr->sh_type == SHT_DYNAMIC)
    ret = elfsh_reloc_dynamic(sect, diff);
  else if (!strcmp(sect->name, ELFSH_SECTION_NAME_CTORS))
    ret = elfsh_reloc_ctors(sect, diff);
  else if (!strcmp(sect->name, ELFSH_SECTION_NAME_DTORS))
    ret = elfsh_reloc_dtors(sect, diff);
  else if (!strcmp(sect->name, ELFSH_SECTION_NAME_GOT))
    ret = elfsh_reloc_got(sect, diff);
  else if (sect->shdr->sh_addr != NULL)
    ret = elfsh_reloc_raw(sect, diff);
  if (ret < 0)
    return (-1);
  return (ret);
}



/* 
** XXX .::. TODO for ET_EXEC to ET_DYN : 
** - Insertion a new .rel section
** - Change objtype and entryp in ELF header
** - Relocate PHT and SHT
** - Change reloc section size in .dynamic *
*/
int		elfsh_remap(elfshobj_t *file, Elf32_Addr new_addr)
{
  elfshsect_t	*sect;
  u_int		diff;
  u_int		count;
  int		ret;

  count = 0;
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:relocate] Invalid NULL parameter\n", -1);
  else if (elfsh_read_obj(file) < 0)
    return (-1);
  diff = elfsh_get_object_baseaddr(file);
  if (diff == (u_int) -1);
    ELFSH_SETERROR("[libelfsh:relocate] Object base address is NULL\n", -1);
  for (sect = file->sectlist; sect != NULL; sect = sect->next)
    {
      ret = elfsh_relocate_section(sect, -diff);
      printf("Relocation number found for %-20s : %d \n", sect->name, ret);
      if (ret < 0)
	return (-1);
      count += ret;
    }
  return (count);
}
