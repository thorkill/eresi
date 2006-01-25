/*
** fixup.c for elfsh
** 
** Started on  Fri Jul 27 04:56:06 2001 mayhem
**
*/
#include "libelfsh.h"



/* TERMINAL FUNCTION ! Fixup size for section symbols, if symtab doesnt exist, create it */
/* This function only works on FILE and is not e2dbg safe */
elfshsect_t	*elfsh_fixup_symtab(elfshobj_t *file, int *strindex)
{
  elfshsect_t	*symtab;
  elfshsect_t	*strtab;
  elfshsect_t	*got;
  elfshsect_t	*list;
  elfshsect_t	*parent;
  elfsh_Shdr	hdr;
  elfsh_Sym	new;
  elfsh_Sym	*actual;
  elfsh_Addr	startaddr;
  u_int		index;
  char		*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Create symbol table if it does not exist */
  if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL)
    {
      XALLOC(symtab, sizeof (elfshsect_t), NULL);
      hdr = elfsh_create_shdr(0, SHT_SYMTAB, 0, 0, 0, 0, 0, 0, 0, sizeof(elfsh_Sym));
      symtab->name = elfsh_strdup(ELFSH_SECTION_NAME_SYMTAB);
      index = elfsh_insert_unmapped_section(file, symtab, hdr, NULL);
      if (index < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to insert SYMTAB", NULL);
      file->secthash[ELFSH_SECTION_SYMTAB] = symtab;
    }
  else
    symtab = file->secthash[ELFSH_SECTION_SYMTAB];

  /* Load strtab */
  strtab = elfsh_get_strtab(file, -1);

  elfsh_set_section_link(symtab->shdr, strtab->index);
  if (strindex != NULL)
    *strindex = strtab->index;

  /* Endianize symbol table */
  symtab->shdr->sh_entsize = sizeof(elfsh_Sym);
  elfsh_endianize_symtab(symtab);

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

  /* Fixup all 0 length non-section-typed symbols */
  actual = symtab->data;
  for (index = 0; 
       index < symtab->shdr->sh_size / sizeof(elfsh_Sym); 
       index++, actual++)
    if (elfsh_get_symbol_type(actual) != STT_BLOCK && 
	actual->st_value && !actual->st_size && 
	index + 1 < symtab->shdr->sh_size / sizeof(elfsh_Sym))
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

  /* _GLOBAL_OFFSET_TABLE_ injection if not present */
  got = elfsh_get_section_by_name(symtab->parent, ELFSH_SECTION_NAME_GOT, 
				  NULL, NULL, NULL);
  actual = elfsh_get_symbol_by_name(symtab->parent, ELFSH_GOTSYM);
  if (actual == NULL && got != NULL)
    {
      new = elfsh_create_symbol(got->shdr->sh_addr, 0, STT_OBJECT,
				0, 0, got->index);
      elfsh_insert_symbol(symtab, &new, ELFSH_GOTSYM);
    }

  /* Fix sctndx */
  if (elfsh_fixup_sctndx(symtab) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to fixup sctndx", NULL);
  
  /* Resynchronize cached sorted symtab and return */
  elfsh_sync_sorted_symtab(symtab);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (symtab));
}




/* Recompute .dynsym symbols address which are zeroed by new versions of ld */
elfsh_Sym	*elfsh_restore_dynsym(elfshobj_t *file, elfshsect_t *plt, 
				      u_int off, elfshsect_t *dynsym)
				     
{
  u_int		entsz;
  uint32_t	index;
  elfsh_Rel	*reldyn;
  elfsh_Sym	*sym;
  elfshsect_t	*relplt;
  char		*name;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Grab values */
  entsz = elfsh_get_pltentsz(file);
  sym = NULL;

  /* Get the PLT related relocation table */
  name = IS_REL(plt) ? ELFSH_SECTION_NAME_RELPLT : ELFSH_SECTION_NAME_RELAPLT;
  relplt = elfsh_get_section_by_name(plt->parent, name, 0, 0, 0);
  if (!relplt)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find RELPLT", NULL);

  /* On Sparc, some of the first entries are reserved */
  if (FILE_IS_SPARC(plt->parent) && off < elfsh_get_first_pltentsz(file))
    {

#if __DEBUG_COPYPLT__
      printf("[DEBUG_COPYPLT] Passing reserved PLT entry on SPARC\n");
#endif

      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Not patching reserved PLT entry", NULL);
    }
  
  /* On some architecture the first plt entry size is different than entsz */
  index = (off - elfsh_get_first_pltentsz(file) + entsz) / entsz;
  if (!index)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Not patching first PLT entry", NULL);
  index--;
  
#if __DEBUG_COPYPLT__
  printf("[DEBUG_COPYPLT] New gnu-ld detected -> looking for reloc index = %u \n", index);
#endif
  
  /* Necessary for get_relent_by_index who does not know about IS_REL */
  elfsh_setrel(IS_REL(plt));
  
  if (IS_REL(plt) ? 
      (sizeof(elfsh_Rel)  * index) >= relplt->shdr->sh_size : 
      (sizeof(elfsh_Rela) * index) >= relplt->shdr->sh_size)
    {

#if __DEBUG_COPYPLT__
      printf("[DEBUG_COPYPLT] Found END of relocation table, symbol not inserted \n");
#endif

      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Not patching PLT entry without relocation entry", NULL);
    }
  
  /* Fixup missing .dynsym value */
  reldyn = elfsh_get_relent_by_index(relplt->data, index);
  index = elfsh_get_relsym(reldyn);
  if (reldyn)
    {
      sym = elfsh_get_symbol_by_index(dynsym->data, index);
      if (sym && !sym->st_value)
	sym->st_value = plt->shdr->sh_addr + off;
    }
  
  /* If still not found */
  if (sym == NULL || sym->st_value == NULL)
    {
      
#if __DEBUG_COPYPLT__
      printf("[DEBUG_COPYPLT] Cannot find symbol at PLT OFFSET + %u (.plt + off = " XFMT " ) object %s \n", 
	     off, (elfsh_Addr) plt->shdr->sh_addr + off, plt->parent->name);
#endif

      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Not patching PLT entry without valid symbol", NULL);
    }

#if __DEBUG_COPYPLT__
      printf("[DEBUG_COPYPLT] Found symbol [%s] for PLT OFFSET + %u (.plt + off = " XFMT " ) object %s\n", 
	     elfsh_get_dynsymbol_name(file, sym), off, (elfsh_Addr) plt->shdr->sh_addr + off, plt->parent->name);
#endif


  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sym);
}




/* Fixup the dynamic symbol table (recompute all zeroed symbols) */
/* Useful on recent versions of ld */
int			elfsh_fixup_dynsymtab(elfshsect_t *dynsym)
{
  elfshsect_t		*plt;
  u_int			off;
  u_int			entsz;
  elfsh_Sym		*sym;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get PLT */
  plt = elfsh_get_plt(dynsym->parent, NULL);
  if (!plt)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get PLT", -1);

  entsz = elfsh_get_pltentsz(plt->parent);

  /* Loop on .plt and inject 'old_symnam' symbols */
  for (off = 0; off < plt->shdr->sh_size; off += entsz)
    {

      /* Special case for the first plt entry */
      if (!off)
	{
	  off = off - entsz + elfsh_get_first_pltentsz(dynsym->parent);
	  continue;
	}
      
      /* Get the existing symbol name for this plt entry ... */
      sym = elfsh_get_sym_by_value(dynsym->data,
				   dynsym->shdr->sh_size / sizeof(elfsh_Sym),
				   plt->shdr->sh_addr + off, NULL, ELFSH_EXACTSYM);
      
      /* New versions of ld do not fill the vaddr of dynamic symbols, do it ourself */
      if (sym == NULL)
	elfsh_restore_dynsym(dynsym->parent, plt, off, dynsym);
    }
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

