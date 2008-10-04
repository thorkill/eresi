/**
 * @file remap.c
 * @ingroup libelfsh
** remap.c for elfsh
** 
** elfsh_reloc_pht/sht/symtab() are based on spacewalker's 
** original modremap.c
** 
** Started on  Mon Mar 17 09:30:33 2003 jfv
** 
**
** $Id: remap.c,v 1.6 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"


/**
 * Remap the Program Header Table 
 * @param file
 * @param diff
 * @return
 */
int		elfsh_reloc_pht(elfshobj_t *file, eresi_Addr diff)
{
  unsigned int		i;
  unsigned int		count;
  eresi_Addr	base;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  base = elfsh_get_object_baseaddr(file);
  if (file == NULL || file->pht == NULL || file->hdr->e_phnum == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", 0);

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

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (count));
}

/**
 * Remap the Section Header Table 
 * @param file
 * @param diff
 * @return
 */
int		elfsh_reloc_sht(elfshobj_t *file, eresi_Addr diff)
{
  unsigned int		i;
  unsigned int		count;
  eresi_Addr	base;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || file->sht == NULL || file->hdr->e_shnum == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  base = elfsh_get_object_baseaddr(file);
  for (count = i = 0; i < file->hdr->e_shnum; i++)
    if (file->sht[i].sh_addr > base)
      {
	file->sht[i].sh_addr += diff;
	count++;
      }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (count));
}

/**
 * Remap a section which type is SHT_SYMTAB or SHT_DYNSYM 
 * @param s
 * @param diff
 * @return
 */
int		elfsh_reloc_symtab(elfshsect_t *s, eresi_Addr diff)
{
  elfsh_Sym	*symtab;
  unsigned int		i;
  eresi_Addr   	vaddr;
  unsigned int		count;
  eresi_Addr	base;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (s == NULL || s->shdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (s->shdr->sh_type != SHT_SYMTAB && s->shdr->sh_type != SHT_DYNSYM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section is not a symbol table", -1);

  symtab = elfsh_get_raw(s);
  base = elfsh_get_object_baseaddr(s->parent);
  for (count = i = 0; i < s->shdr->sh_size / sizeof(elfsh_Sym); i++)
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
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (count));
}

/**
 * Remap a section using its extra relocation entries 
 * @param cur
 * @param diff
 * @return
 */
int		elfsh_reloc_raw(elfshsect_t *cur, eresi_Addr diff)
{
  unsigned int		index;
  eresi_Addr	addr;
  elfshsect_t	*target;
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (cur == NULL || cur->shdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  if (elfsh_get_raw(cur) == NULL || cur->rel == NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

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
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Invalid IDX_DST",  -1);

	str = elfsh_get_raw(cur) + cur->rel[index].off_src;
	addr = target->shdr->sh_addr + cur->rel[index].off_dst + diff;
	memcpy(str, &addr, sizeof(eresi_Addr));

	/* Do not relocate */
      case ELFSH_RELOC_FP:
      default:
	break;
      }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (cur->srcref));
}

/**
 * Remap the .dynamic section 
 * @param sect
 * @param diff
 * @return
 */
int		elfsh_reloc_dynamic(elfshsect_t *sect, eresi_Addr diff)
{
  elfshsect_t	*parent;
  elfsh_Dyn	*dyn;
  unsigned int		index;
  unsigned int		count;
  eresi_Addr	val;
  unsigned int		nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sect == NULL || sect->shdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (sect->shdr->sh_type != SHT_DYNAMIC)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unexpected section type", -1);

  nbr = sect->shdr->sh_size / sizeof(elfsh_Dyn);
  for (dyn = elfsh_get_raw(sect), count = index = 0; index < nbr; index++)
    {
      val = elfsh_get_dynentry_val(dyn + index);
      parent = elfsh_get_parent_section(sect->parent, val, NULL);
      if (val && parent != NULL && parent->shdr->sh_addr != NULL)
	{
	  elfsh_set_dynentry_val(dyn + index, val + diff);
	  count++;
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (count));
}


/**
 * Remap sections of type SHT_REL and SHT_RELA 
 * @param sect
 * @param diff
 * @return
 */
int		elfsh_reloc_rel(elfshsect_t *sect, eresi_Addr diff)
{
  elfshsect_t	*parent;
  elfsh_Rel	*rel;
  unsigned int		index;
  unsigned int		count;
  unsigned int		nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sect == NULL || sect->shdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (sect->shdr->sh_type != SHT_REL && sect->shdr->sh_type != SHT_RELA)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unexpected section type", -1);

  nbr = sect->shdr->sh_size / sizeof(elfsh_Rel);
  for (rel = elfsh_get_raw(sect), count = index = 0; index < nbr; index++)
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
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (count));
}

/**
 * Remap section's whoose type is a data array (GOT, CTORS, DTORS ..) 
 * @param file
 * @param array
 * @param size
 * @param diff
 * @return
 */
int		elfsh_reloc_array(elfshobj_t *file, 
				  eresi_Addr *array, 
				  unsigned int      size, 
				  eresi_Addr diff)
{
  elfshsect_t	*parent;
  unsigned int		index;
  unsigned int		count;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || array == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL paramater", -1);

  for (count = index = 0; index < size; index++)
    {
      parent = elfsh_get_parent_section(file, array[index], NULL);
      if (parent != NULL && parent->shdr->sh_addr != NULL && array[index] != NULL)
	{  
	  array[index] += diff;
	  count++;
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (count));
}

/**
 * Remap Global Offset Table
 * @param sect
 * @param diff
 * @return
 */
int		elfsh_reloc_got(elfshsect_t *sect, eresi_Addr diff)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sect == NULL || sect->shdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (strcmp(sect->name, ELFSH_SECTION_NAME_GOT))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unexpected section name", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_reloc_array(sect->parent, elfsh_get_raw(sect), 
			    sect->shdr->sh_size / sizeof(eresi_Addr), diff)));
}

/**
 * Remap .ctors section 
 * @param sect
 * @param diff
 * @return
 */
int		elfsh_reloc_ctors(elfshsect_t *sect, eresi_Addr diff)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sect == NULL || sect->shdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (strcmp(sect->name, ELFSH_SECTION_NAME_CTORS))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unexpected section name", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_reloc_array(sect->parent, elfsh_get_raw(sect), 
			    sect->shdr->sh_size / sizeof(eresi_Addr), 
			    diff)));
}

/**
 * Remap .dtors section 
 * @param sect
 * @param diff
 * @return
 */
int		elfsh_reloc_dtors(elfshsect_t *sect, eresi_Addr diff)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sect == NULL || sect->shdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (strcmp(sect->name, ELFSH_SECTION_NAME_DTORS))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unexpected section name", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_reloc_array(sect->parent, elfsh_get_raw(sect), 
			    sect->shdr->sh_size / sizeof(eresi_Addr), 
			    diff)));
}


/**
 * Not used ATM since it triggers more false positives ;P 
 * @param sect
 * @param diff
 * @return
 */
int		elfsh_reloc_hash(elfshsect_t *sect, eresi_Addr diff)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sect == NULL || sect->shdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (sect->shdr->sh_type != SHT_HASH)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section is not HASH", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_reloc_array(sect->parent, elfsh_get_raw(sect), 
			    sect->shdr->sh_size / sizeof(eresi_Addr), diff)));
}


/**
 * Call the type dependant remapping routine for this section 
 * @param sect
 * @param diff
 * @return
 */
int		elfsh_relocate_section(elfshsect_t *sect, eresi_Addr diff)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = 0;
  if (sect == NULL || elfsh_get_raw(sect) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL paramater", -1);

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
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Relocation recovery failed for this section", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/** 
 * XXX .::. TODO for ET_EXEC to ET_DYN : 
 * - Insertion a new .rel section
 * - Change objtype and entryp in ELF header
 * - Relocate PHT and SHT
 * - Change reloc section size in .dynamic *
 *
 * @param file
 * @param new_addr
 * @return
 */
int		elfsh_remap(elfshobj_t *file, eresi_Addr new_addr)
{
  elfshsect_t	*sect;
  eresi_Addr	diff;
  unsigned int		count;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  count = 0;
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (elfsh_read_obj(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to read object", -1);

  diff = elfsh_get_object_baseaddr(file);
  if (diff == (eresi_Addr) -1);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Object base address is NULL", -1);

  for (sect = file->sectlist; sect != NULL; sect = sect->next)
    {
      ret = elfsh_relocate_section(sect, -diff);
      printf("Relocation number found for %-20s : %d \n", sect->name, ret);
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to relocate section", -1);
      count += ret;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (count));
}
