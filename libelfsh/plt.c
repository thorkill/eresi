/*
** plt.c for libelfsh
** 
** Started on  Tue Jul 24 06:40:07 2001 mayhem
** Last update Wed Jul  2 18:00:05 2003 mayhem
*/
#include "libelfsh.h"


/* 
** Copy the .plt so that we can redirect original .plt, 
** keep the original .plt and make old_symbolname points on it
*/
int		elfsh_copy_plt(elfshobj_t *file)
{
  elfshsect_t   *plt;
  elfshsect_t	*new;
  elfshsect_t	*symtab;
  elfshsect_t	*dynsym;
  Elf32_Shdr	hdr;
  Elf32_Sym	*sym;
  Elf32_Sym	newsym;
  char		buf[BUFSIZ];
  u_int		off;
  u_int		entsz;
  int		mode;

  /* Sanity checks */
  if (file->secthash[ELFSH_SECTION_ALTPLT] != NULL)
    return (0);
  plt = elfsh_get_plt(file, NULL);
  if (NULL == plt)
    return (-1);
  entsz = elfsh_get_pltentsz(file);
  if (entsz < 0)
    return (-1);
  if (NULL == elfsh_get_dynsymtab(file, NULL))
    return (-1);
  if (NULL == elfsh_get_symtab(file, NULL))
    return (-1);

  /* Insert alternative .plt */
  dynsym = file->secthash[ELFSH_SECTION_DYNSYM];
  symtab = file->secthash[ELFSH_SECTION_SYMTAB];
  new = elfsh_create_section(ELFSH_SECTION_NAME_ALTPLT);
  hdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC, 
			  0, 0, plt->shdr->sh_size, 0, 0, 0, 0);

  /* FreeBSD is incompatible with pre-interp injection */
#ifdef __FreeBSD__
  mode = ELFSH_DATA_INJECTION;
#else
  mode = ELFSH_CODE_INJECTION;
#endif

  if (elfsh_insert_mapped_section(file, new, hdr, plt->data, mode) < 0)
    return (-1);
  new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTPLT, 
				  NULL, NULL, NULL);
  if (new == NULL)
    return (-1);
  file->secthash[ELFSH_SECTION_ALTPLT] = new;


  /* Loop on .plt and inject 'old_symnam' symbols */
  for (off = 0; off < plt->shdr->sh_size; off += entsz)
    {

      /* Special case for the first plt entry */
      if (off == 0)
	{
	  newsym = elfsh_create_symbol(new->shdr->sh_addr, 
				       entsz, STT_FUNC, 0, 
				       0, 0);
	  if (elfsh_insert_symbol(symtab, &newsym, "old_dlresolve") < 0)
	    return (-1);
	  if (FILE_IS_SPARC(file) && 
	      elfsh_hijack_plt_sparc_g2(file, &newsym,
					plt->shdr->sh_addr) < 0)
	    return (-1);
	  continue;
	}

      /* Else we get the existing symbol name for this plt entry ... */
      sym = elfsh_get_sym_by_value(dynsym->data,
				   dynsym->shdr->sh_size / sizeof(Elf32_Sym),
				   plt->shdr->sh_addr + off,
				   NULL, ELFSH_EXACTSYM);
      if (sym == NULL)
	{
#if __DEBUG_COPYPLT__
	  printf("[DEBUG_COPYPLT] Cannot find symbol at .plt + %u \n", off);
#endif
	  continue;
	}

      /* ... and we inject the 'old' occurence symbol pointing in .orig.plt */
      newsym = elfsh_create_symbol(new->shdr->sh_addr + off, 
				   entsz, STT_FUNC, 0, 0, 0);
      snprintf(buf, BUFSIZ, "old_%s", elfsh_get_dynsymbol_name(file, sym));
      if (elfsh_insert_symbol(symtab, &newsym, buf) < 0)
	return (-1);

#if __DEBUG_COPYPLT__
      printf("[DEBUG_COPYPLT] Symbol at .plt + %u injected succesfully \n", off);
#endif

    }

  /* Everything is 0k4y */
  if (elfsh_sync_sorted_symtab(symtab) < 0)
    return (-1);
  return (0);
}




/* Load the Procedure Linkage Table */
elfshsect_t	*elfsh_get_plt(elfshobj_t *file, int *num)
{
  elfshsect_t	*actual;
  void		*ret;

  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    return (NULL);

  for (actual = file->sectlist; actual != NULL; actual = actual->next)
    if (elfsh_is_plt(file, actual))
      {
	ret = elfsh_get_anonymous_section(file, actual);
	file->secthash[ELFSH_SECTION_PLT] = actual;
	return (ret ? actual : NULL);
      }
  
  return (NULL);
}


/* Return 1 if the section is the plt or 0 if its not */
int		elfsh_is_plt(elfshobj_t *file, elfshsect_t *sct)
{
  char		*name;

  name = elfsh_get_section_name(file, sct);
  if (name != NULL && !strcmp(name, ELFSH_SECTION_NAME_PLT))
    return (1);
  return (0);
}

/* Return 1 if the symbol points in the PLT, 0 if not, -1 if the PLT cant be found */
int			elfsh_is_pltentry(elfshobj_t *file, Elf32_Sym *sym)
{
  if (sym == NULL || elfsh_get_plt(file, NULL) == NULL)
    ELFSH_SETERROR("[libelfsh:is_pltentry] Invalid NULL parameter\n", -1);
  if (sym->st_size != file->secthash[ELFSH_SECTION_PLT]->shdr->sh_size)
    if (INTERVAL(file->secthash[ELFSH_SECTION_PLT]->shdr->sh_addr, 
		 sym->st_value, 
		 file->secthash[ELFSH_SECTION_PLT]->shdr->sh_addr + 
		 file->secthash[ELFSH_SECTION_PLT]->shdr->sh_size))
      return (1);
  return (0);
}


/* Return the size of 1 PLT entry */
int		elfsh_get_pltentsz(elfshobj_t *file)
{
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_pltentsz] Invalid NULL parameter\n", -1);
  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      /* XXX: case EM_486: */
      return (16);
    case EM_SPARC:
    case EM_SPARC32PLUS:
      return (12);
    default:
      ELFSH_SETERROR("[libelfsh:get_pltentsz] Unsupported architecture\n", -1);
    }
}





























