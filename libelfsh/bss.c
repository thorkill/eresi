/*
** bss.c for elfsh
**
** Started on  Sun Apr  6 08:35:07 2003 mayhem
** Last update Fri Aug 15 23:26:13 2003 jv
*/
#include "libelfsh.h"




/* Create and insert a 4 bytes BSS section in file */
elfshsect_t		*elfsh_rebuild_bss(elfshobj_t *file)
{
  elfshsect_t		*bss;
  Elf32_Shdr		hdr;

  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:add_bss] Invalid NULL parameter\n", NULL);

  /* Create section */
  bss = elfsh_create_section(ELFSH_SECTION_NAME_BSS);
  if (bss != NULL)
    return (bss);

  /* Create header */
  hdr = elfsh_create_shdr(0, SHT_NOBITS, SHF_WRITE | SHF_ALLOC, 0, 0, 4, 0, 0, 0, 0);

  /* Inject new section */
  if (elfsh_insert_data_section(file, bss, hdr, NULL) < 0)
    ELFSH_SETERROR("[libelfsh:add_bss] Cant insert .bss\n", NULL);
  return (0);
}




/* Put the bss physically in the file */
elfshsect_t	*elfsh_fixup_bss(elfshobj_t *file)
{
  elfshsect_t	*bss;
  elfshzone_t	*orig;

  elfshsect_t	*next;
  u_int		size;


  /* Find the bss or reconstruct it */
  bss = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_BSS,
				  NULL, NULL, NULL);
  if (bss == NULL) /* && NULL == elfsh_rebuild_bss(file)) */
    return (NULL);

  if (bss->shdr->sh_size == 0)
    ELFSH_SETERROR("[libelfsh:fixup_bss] .bss section size == 0\n", NULL);

  /* Fixup file offset for section after bss in case they overlap */
  /* Happens on Solaris 8.0 with ld solaris-ELF 4.0 */
  if (bss->next != NULL)
    if (bss->next->shdr->sh_offset <= bss->shdr->sh_offset + bss->shdr->sh_size)
      {
	size = bss->shdr->sh_offset + bss->shdr->sh_size - bss->next->shdr->sh_offset;
	file->hdr->e_shoff += size;
	for (next = bss->next; next != NULL; next = next->next)
	  next->shdr->sh_offset += size;
      }

  /* Add root bss zone */
  if (bss->data == NULL)
    {
      orig = elfsh_create_bsszone(bss->parent->name, 0, bss->shdr->sh_size);
      if (orig == NULL || elfsh_add_bsszone(bss, orig) < 0)
	return (NULL);
    }
  return (bss);
}



/* Create a new BSS zone */
elfshzone_t	*elfsh_create_bsszone(char *name, u_int off, u_int size)
{
  elfshzone_t	*new;

  XALLOC(new, sizeof(elfshzone_t), NULL);
  new->modname = name;
  new->off = off;
  new->size = size;
  return (new);
}



/* Add BSS zone at the end of the section */
int		elfsh_add_bsszone(elfshsect_t *bss, elfshzone_t *zone)
{
  elfshzone_t	*act;
  elfshsect_t	*actual;

  /* Sanity checks */
  if (bss == NULL || zone == NULL || bss->shdr == NULL)
    ELFSH_SETERROR("[libelfsh:add_bsszone] Invalid NULL parameter\n", -1);
  else if (bss->shdr->sh_type != SHT_NOBITS)
    ELFSH_SETERROR("[libelfsh:add_bsszone] Section is not BSS\n", -1);

  /* Create initial zone */
  if (bss->modzone == NULL)
    bss->modzone = zone;

  /* Put the new zone at the end */
  else
    {

      /* Increase BSS size */
      act = bss->modzone;
      while (act->next != NULL)
	act = act->next;
      act->next = zone;
      bss->shdr->sh_size += zone->size;
    }


#if __DEBUG_BSS__
  printf("[DEBUG_RELBSS] %-20s [%08u] for new BSS zone (new upper bound = %08X, zonesize = %08u) \n",
	 (bss->data == NULL ? "Allocating" : "Reallocating"),
	 (u_int) bss->shdr->sh_size,
	 (u_int) (bss->shdr->sh_addr + bss->shdr->sh_size),
	 (u_int) zone->size);
#endif


  /* Allocate BSS zone data physically */
  if (bss->data == NULL)
    {
      XALLOC(bss->data, zone->size, -1);
      if (bss->phdr == NULL)
	bss->phdr = bss->prev->phdr;
      bss->phdr->p_filesz = bss->phdr->p_memsz;
    }


  /* Extend physical bss and the segment for .bss if PHT exists */
  else
    {
      XREALLOC(bss->data, bss->data, bss->shdr->sh_size, -1);
      bss->phdr->p_filesz += zone->size;
      bss->phdr->p_memsz += zone->size;
    }

  /* Update the SHT file offset if necessary */
  if (bss->parent->hdr->e_shoff > bss->shdr->sh_offset)
    bss->parent->hdr->e_shoff += zone->size;

  /* Update the vaddr and file offset for all next sections */
  for (actual = bss->next; actual != NULL; actual = actual->next)
    {
      actual->shdr->sh_offset += zone->size;
      if (actual->shdr->sh_addr)
	actual->shdr->sh_addr += zone->size;
    }

  return (0);
}



/* Find BSS zone by name */
elfshzone_t	*elfsh_find_bsszone(elfshsect_t *bss, char *name)
{
  elfshzone_t	*cur;

  /* Sanity checks */
  if (bss == NULL || name == NULL)
    ELFSH_SETERROR("[libelfsh:find_bsszone] Invalid NULL parameter\n", NULL);
  else if (bss->shdr->sh_type != SHT_NOBITS || bss->modzone == NULL)
    ELFSH_SETERROR("[libelfsh:find_bsszone] Invalid NULL parameter\n", NULL);

  /* Find zone */
  for (cur = bss->modzone; cur != NULL; cur = cur->next)
    if (!strcmp(cur->modname, name))
      return (cur);

  /* Not found */
  return (NULL);
}



/* Find BSS size */
int		elfsh_find_bsslen(elfshobj_t	*host,
				  elfshobj_t	*rel)
{
  elfshsect_t	*host_bss;
  elfshsect_t	*host_symtab;
  Elf32_Sym	*symtab;
  Elf32_Sym	new;
  u_int		size;
  u_int		index;
  char		*name;
  u_int		bss_size;

  /* Sanity checks */
  if (host == NULL || rel == NULL)
    ELFSH_SETERROR("[libelfsh:find_bsslen] Invalid NULL parameter\n", -1);

  /* Init some stuff */
  elfsh_get_symtab(host, NULL);
  host_symtab = host->secthash[ELFSH_SECTION_SYMTAB];
  symtab = elfsh_get_symtab(rel, &size);
  host_bss = elfsh_fixup_bss(host);
  if (symtab == NULL || host_bss == NULL || host_symtab == NULL)
    return (-1);
  bss_size = host_bss->shdr->sh_addr + host_bss->shdr->sh_size;

  /* Parse symtab and find all COMMON symbols */
  for (index = 0; index < size; index++)
    if (elfsh_get_symbol_link(symtab + index) == SHN_COMMON)
      {

	/* Align symbol size as needed */
	while (bss_size % symtab[index].st_value)
	  bss_size++;

	/* Create and inject symbol in ET_EXEC */
	new = elfsh_create_symbol(bss_size,
				  symtab[index].st_size,
				  STT_OBJECT, STB_LOCAL,
				  0, host_bss->index);

	name = elfsh_get_symbol_name(rel, symtab + index);
	elfsh_insert_symbol(host_symtab, &new, name);
	bss_size += symtab[index].st_size;
      }

  bss_size -= host_bss->shdr->sh_addr + host_bss->shdr->sh_size;

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Found BSS zone lenght [%08u] for module [%s] \n",
	 bss_size, rel->name);
#endif

  elfsh_sync_sorted_symtab(host_symtab);
  return (bss_size);
}
