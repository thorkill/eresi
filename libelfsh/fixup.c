/*
** fixup.c for elfsh
** 
** Started on  Fri Jul 27 04:56:06 2001 mayhem
** Last update Sun Jun 29 11:17:39 2003 mayhem
*/
#include "libelfsh.h"



/* TERMINAL FUNCTION ! Fixup size for section symbols, if symtab doesnt exist, create it */
elfshsect_t	*elfsh_fixup_symtab(elfshobj_t *file, int *strindex)
{
  elfshsect_t	*symtab;
  elfshsect_t	*strtab;
  elfshsect_t	*list;
  elfshsect_t	*parent;
  Elf32_Shdr	hdr;
  Elf32_Sym	new;
  Elf32_Sym	*actual;
  u_long	startaddr;
  u_int		index;
  char		*str;

  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:fixup_symtab] Invalid NULL parameter\n", NULL);

  /* Create symbol table if it does not exist */
  if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL)
    {
      XALLOC(symtab, sizeof (elfshsect_t), NULL);
      hdr = elfsh_create_shdr(0, SHT_SYMTAB, 0, 0, 0, 0, 0, 0, 0, 0);
      symtab->name = strdup(ELFSH_SECTION_NAME_SYMTAB);
      index = elfsh_insert_unmapped_section(file, symtab, hdr, NULL);
      if (index < 0)
	return (NULL);
      file->secthash[ELFSH_SECTION_SYMTAB] = symtab;
    }
  else
    symtab = file->secthash[ELFSH_SECTION_SYMTAB];

  /* Load strtab */
  strtab = elfsh_get_strtab(file, -1);
  elfsh_set_section_link(symtab->shdr, strtab->index);
  if (strindex != NULL)
    *strindex = strtab->index;

  /* Walk to the sht and fixup the symbol table */
  for (list = file->sectlist; list != NULL; list = list->next)
    {

      /* Create STT_SECTION symbol if unexistant */
      actual = elfsh_get_sym_from_shtentry(file, list->shdr);
      if (actual == NULL)
        elfsh_insert_sectsym(file, list);  

      /* Fixup STT_SECTION symbol */
      else
	{
	  actual->st_size = list->shdr->sh_size;
	  str = elfsh_get_symbol_name(file, actual);
	  if (str == NULL || !*str)
	    actual->st_name = elfsh_insert_in_strtab(file, list->name);
	}
    }

  /* Fixup all 0 lenght non-section-typed symbols */
  actual = symtab->data;
  for (index = 0; 
       index < symtab->shdr->sh_size / sizeof(Elf32_Sym); 
       index++, actual++)
    if (elfsh_get_symbol_type(actual) != STT_BLOCK && 
	actual->st_value && !actual->st_size && 
	index + 1 < symtab->shdr->sh_size / sizeof(Elf32_Sym))
      actual->st_size = actual[1].st_value - actual->st_value;

  /* Fixup _start symbol value [and create it if unexistant] */
  actual = elfsh_get_symbol_by_name(symtab->parent, ELFSH_STARTSYM);
  if (actual == NULL)
    {
      startaddr = elfsh_get_entrypoint(symtab->parent->hdr);
      parent = elfsh_get_parent_section(symtab->parent, startaddr, NULL);
       if (parent != NULL)
	 {
	    new = elfsh_create_symbol(startaddr, 0, STT_FUNC,
				      0, 0, parent->index);
	    elfsh_insert_symbol(symtab, &new, ELFSH_STARTSYM);
	 }
    }
  else
    {
      elfsh_set_symbol_size(actual, 0);
      elfsh_set_symbol_type(actual, STT_FUNC);
    }

  /* Resynchronize cached sorted symtab and return */
  elfsh_sync_sorted_symtab(symtab);
  return (symtab);
}





