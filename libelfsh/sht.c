/*
** sht.c for libelfsh
**
** Started on  Mon Feb 26 04:16:18 2001 mayhem
** Last update Thu Aug 21 03:38:03 2003 mayhem
*/
#include "libelfsh.h"




/* Sort SHT by file offset */
/* Mandatory on gcc 2.95.2/2.96 generated bins, maybe others */
int			elfsh_sort_sht(elfshobj_t *file)
{
  elfshsect_t		*actual;
  elfshsect_t		*tmpsct;
  elfshsect_t		*next;
  Elf32_Shdr		tmp;
  u_int			index;
  u_int			pass;

  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:sort_sht] Invalid NULL parameter\n", -1);
  else if (file->sht == NULL && NULL == elfsh_get_sht(file, NULL))
    return (-1);

  /* Basic bubble sort like : O(n2) */
  for (pass = 0; pass < file->hdr->e_shnum; pass++)
    {

#if __DEBUG_SORT__
      printf(" ****** Pass %02u ******* \n", pass);
      fflush(stdout);
#endif

      for (actual = file->sectlist, index = 0;
	   index + 1 < file->hdr->e_shnum;
	   index++, actual = next)
	{

#if __DEBUG_SORT__
	  elfsh_print_sectlist(file);
#endif

	  next = actual->next;
	  if (file->sht[index].sh_offset > file->sht[index + 1].sh_offset)
	    {

	      /* Swap SHT entries */
	      tmp = file->sht[index];
	      file->sht[index] = file->sht[index + 1];
	      file->sht[index + 1] = tmp;
	      actual->shdr++;
	      actual->next->shdr--;

	      /* Change index field of current section */
	      actual->index++;
	      actual->next->index--;

	      /* Update next pointer cause sections are going to be swapped */
	      next = actual;

	      /* Now swap in 2 first sections in the list */
	      if (index == 0)
		{
		  file->sectlist = actual->next;
		  tmpsct = file->sectlist->next;
		  file->sectlist->next = actual;
		  file->sectlist->prev = file->sectlist;
		  file->sectlist->next->prev = file->sectlist;
		  file->sectlist->next->next = tmpsct;
		  file->sectlist->next->next->next->prev = actual;
		}

	      /* If not in first position */
	      else
		{
		  tmpsct = actual->prev;
		  actual->prev->next = actual->prev = actual->next;
		  actual->next->prev = tmpsct;
		  if (actual->next->next != NULL)
		    actual->next->next->prev = actual;
		  tmpsct = actual->next->next;
		  actual->next->next = actual;
		  actual->next = tmpsct;
		}

	      /* Update links in SHT and symlinksidx in symtab */
	      /* Not optimized */
	      elfsh_update_linkidx_equ(file, index, 1);
	      elfsh_update_linkidx_equ(file, index + 1, -1);
	      elfsh_update_symlinkidx_equ(file, index, 1);
	      elfsh_update_symlinkidx_equ(file, index + 1, -1);
	    }
	}

#if __DEBUG_SORT__
      puts("-------------------------------------------");
#endif

    }

#if __DEBUG_SORT__
  puts("Final state :");
  elfsh_print_sectlist(file);
  puts("-------------------------------------------");
#endif

  return (0);
}


/* Duplicate names for sections */
void		elfsh_assoc_name(elfshobj_t *file)
{
  elfshsect_t	*s;
  char		*name;

  for (s = file->sectlist; s != NULL; s = s->next)
    {
      name = elfsh_get_section_name(file, s);
      if (s->name != NULL)
	free(s->name);
      s->name = (name != NULL ? strdup(name) : NULL);
    }
}



/* Return a ptr on the section header table */
void		*elfsh_get_sht(elfshobj_t *file, int *num)
{
  elfshsect_t	*s;

  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_sht] Invalid NULL parameter\n", NULL);
  if (file->sht != NULL)
    goto end;

  /* Load the section header table and allocate every elfshsect_t */
  else if (elfsh_load_sht(file) < 0 && elfsh_rebuild_sht(file) < 0)
    ELFSH_SETERROR("[libelfsh:get_sht] Cant load SHT\n", NULL);

  /* Load .shstrtab if undone in rebuild_sht() continue w/o if we cant read it */
  if (file->secthash[ELFSH_SECTION_SHSTRTAB] == NULL)
    {
      s = elfsh_get_section_by_index(file, file->hdr->e_shstrndx, NULL, NULL);
      if (s != NULL)
	{
	  file->secthash[ELFSH_SECTION_SHSTRTAB] = s;
	  if (s->data == NULL)
	    s->data = elfsh_load_section(file, s->shdr);
	}
    }

  /* Sort SHT entries and section list by their file offset */
  elfsh_assoc_name(file);

  /* Some final stuffs */
 end:
  if (num != NULL)
    *num = file->hdr->e_shnum;
  return (file->sht);
}







/* Return an array of section header */
int		elfsh_load_sht(elfshobj_t *file)
{
  elfshsect_t	*new;
  int		size;
  int		index;
  Elf32_Shdr	*s;
  Elf32_Half	i;

  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  else if (file->sht != NULL)
    return (0);

  /* Read table */
  size = file->hdr->e_shnum * file->hdr->e_shentsize;
  if (!file->hdr->e_shoff)
    ELFSH_SETERROR("[libelfsh] SHT file offset is NULL\n", -1);
  XSEEK(file->fd, file->hdr->e_shoff, SEEK_SET, -1);
  XALLOC(file->sht, size, -1);
  XREAD(file->fd, file->sht, size, -1);

  /* Deal with cross-endianness binaries */
#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (file->hdr->e_ident[EI_DATA] == ELFDATA2MSB) {
#elif __BYTE_ORDER == __BIG_ENDIAN
  if (file->hdr->e_ident[EI_DATA] == ELFDATA2LSB) {
#else
#error Unexpected __BYTE_ORDER !
#endif
    s = file->sht;
    for (i = 0; i < file->hdr->e_shnum; i++)
      {
	s->sh_name = swap32(s->sh_name);
	s->sh_type = swap32(s->sh_type);
	s->sh_flags = swap32(s->sh_flags);
	s->sh_addr = swap32(s->sh_addr);
	s->sh_offset = swap32(s->sh_offset);
	s->sh_size = swap32(s->sh_size);
	s->sh_link = swap32(s->sh_link);
	s->sh_info = swap32(s->sh_info);
	s->sh_addralign = swap32(s->sh_addralign);
	s->sh_entsize = swap32(s->sh_entsize);
	s++;
      }
  }

  /*
  ** Allocate a elfshsect_t per section and put it
  ** in the file->sectlist double linked list
  */
  for (index = 0; index < file->hdr->e_shnum; index++)
    {
      XALLOC(new, sizeof(elfshsect_t), -1);
      size = elfsh_add_section(file, new, index, NULL, ELFSH_SHIFTING_COMPLETE);
      if (size < 0)
	return (-1);
    }
  return (0);
}







/* Retreive the section header giving the section symbol from .symtab */
Elf32_Shdr		*elfsh_get_shtentry_from_sym(elfshobj_t *file,
						     Elf32_Sym	*sym)
{
  elfshsect_t		*sect;

  sect = elfsh_get_section_from_sym(file, sym);
  return (sect == NULL ? NULL : sect->shdr);
}





/* Retreive the symbol for the section giving the section header */
Elf32_Sym		*elfsh_get_sym_from_shtentry(elfshobj_t *file,
						     Elf32_Shdr *hdr)
{
  Elf32_Sym		*current;
  int			index;
  u_int			nbr;

  /* Sanity checks */
  if (file == NULL || hdr == NULL)
    ELFSH_SETERROR("[libelfsh:get_sym_from_shtentry] Invalid NULL parameter\n",
		   NULL);
  else if (file->sht == NULL && NULL == elfsh_get_sht(file, NULL))
    return (NULL);

  /* Load symtab if necessary */
  else if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL &&
	   NULL == elfsh_get_symtab(file, NULL))
    return (NULL);
  else if (!file->secthash[ELFSH_SECTION_SYMTAB]->shdr->sh_size)
    ELFSH_SETERROR("[libelfsh:get_sym_from_shtentry] Symtab is void\n",
		   NULL);
  nbr = file->secthash[ELFSH_SECTION_SYMTAB]->shdr->sh_size / sizeof(Elf32_Sym);

  /* Find the wanted symbol */
  index = 0;
  for (current = file->secthash[ELFSH_SECTION_SYMTAB]->data;
       index < nbr; index++)
    if (elfsh_get_symbol_type(current + index) == STT_SECTION &&
	current[index].st_value == hdr->sh_addr)
      return (current + index);

  ELFSH_SETERROR("[libelfsh:get_sym_from_shtentry] Symbol not found\n", NULL);
}







/* Create a section header */
Elf32_Shdr		elfsh_create_shdr(Elf32_Word name,
					  Elf32_Word type,
					  Elf32_Word flags,
					  Elf32_Addr addr,
					  Elf32_Off offset,
					  Elf32_Word size,
					  Elf32_Word link,
					  Elf32_Word info,
					  Elf32_Word align,
					  Elf32_Word entsize)
{
  Elf32_Shdr		new;

  new.sh_name = name;
  new.sh_type = type;
  new.sh_flags = flags;
  new.sh_addr = addr;
  new.sh_offset = offset;
  new.sh_size = size;
  new.sh_link = link;
  new.sh_info = info;
  new.sh_addralign = align;
  new.sh_entsize = entsize;
  return (new);
}




/* Add a section header to the object */
int		elfsh_insert_section_header(elfshobj_t	*file,
					    Elf32_Shdr	hdr,
					    u_int	range,
					    char	*name)
{
  elfshsect_t	*tmp;
  int		index;
  int		shname;

  /* Preliminary checks */
  if (file == NULL ||
      (file->hdr == NULL && elfsh_get_hdr(file) == NULL) ||
      (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL))
    return (-1);

  /* Fixup the range */
  if (range == ELFSH_SECTION_LAST)
    range = file->hdr->e_shnum;
  else if (range > file->hdr->e_shnum)
    ELFSH_SETERROR("[libelfsh] Invalid range for injection\n", -1);

  /* Insert the name string in .shstrtab */
  shname = elfsh_insert_in_shstrtab(file, name);
  if (shname < 0)
    ELFSH_SETERROR("[libelfsh] Cannot inject data in .shstrtab\n", -1);
  hdr.sh_name = (u_int) shname;
  if (range > file->secthash[ELFSH_SECTION_SHSTRTAB]->index)
    hdr.sh_offset += strlen(name) + 1;

  /* Extend the SHT and fill the new slot */
  XREALLOC(file->sht, file->sht,
	   file->hdr->e_shentsize * (file->hdr->e_shnum + 1), -1);

  if (range != file->hdr->e_shnum)
    memmove(file->sht + range + 1,
	    file->sht + range,
	    (file->hdr->e_shnum - range) * file->hdr->e_shentsize);

  file->sht[range] = hdr;
  file->hdr->e_shnum++;

  /* Update foffset for all section header whoose section is placed after SHT */
  for (tmp = file->sectlist, index = 0; index < file->hdr->e_shnum; index++)
    {

      /*
      ** Since we reallocated the sht, we have to update the shdr pointer
      ** since its pointing to the old chunk !
      ** Pass headers without body (this include the just inserted one)
      */
      if (index != range && tmp)
	{
	  tmp->shdr = file->sht + index;
	  tmp = tmp->next;
	}

      /* Now update the file offset for the actual section */
      if (file->sht[index].sh_offset >= file->hdr->e_shoff)
	file->sht[index].sh_offset += file->hdr->e_shentsize;

    }

  /* If we have an influence on the sht file offset, update it */
  if (hdr.sh_offset <= file->hdr->e_shoff)
    file->hdr->e_shoff += hdr.sh_size;

  /* Update the .shstrtab index in the ELF header if necessary */
  if (range <= file->hdr->e_shstrndx)
    file->hdr->e_shstrndx++;

  return (range);
}




/* Get the section's name in .shstrtab */
char		*elfsh_get_section_name(elfshobj_t *file,
					elfshsect_t *s)
{
  volatile char	*str;

  /* Sanity check */
  if (file == NULL || s == NULL ||
      file->secthash[ELFSH_SECTION_SHSTRTAB] == NULL ||
      (file->sht == NULL && NULL == elfsh_get_sht(file, NULL)))
    return (NULL);

  /* Return section's name */
  str = file->secthash[ELFSH_SECTION_SHSTRTAB]->data;
  return ((char *) str + s->shdr->sh_name);
}




/* Set the section's name in .shstrtab */
int			elfsh_set_section_name(elfshobj_t	*file,
					       elfshsect_t	*s,
					       char		*name)
{
  char		*str;
  u_int		len;
  u_int		new_len;

  /* Sanity check */
  if (file == NULL || s == NULL || name == NULL ||
      file->secthash[ELFSH_SECTION_SHSTRTAB] == NULL ||
      (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL))
    return (-1);

  /* Return section's name */
  str = file->secthash[ELFSH_SECTION_SHSTRTAB]->data;
  str += s->shdr->sh_name;
  len = strlen(str);
  new_len = strlen(name);

  /* Do not allocate new place if possible */
  if (len >= new_len)
    strcpy(str, name);

  /* Append the name to .shstrtab */
  else
    s->shdr->sh_name = elfsh_insert_in_shstrtab(file, name);

  return (s->shdr->sh_name);
}




/* Tell elfsh that we dont want the sht in the output file */
void		elfsh_remove_sht(elfshobj_t *file)
{
  file->shtrm = 1;
}




/* SET/GET the allocatable flag */
char	elfsh_get_section_allocflag(Elf32_Shdr *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return ((s->sh_flags & SHF_ALLOC));
}

char	elfsh_set_section_allocflag(Elf32_Shdr *s, char f)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  if (f)
    s->sh_flags |= SHF_ALLOC;
  else
    s->sh_flags &= (~SHF_ALLOC);
  return (0);
}


/* SET/GET the writable flag */
char	elfsh_get_section_writableflag(Elf32_Shdr *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return ((s->sh_flags & SHF_WRITE));
}

char	elfsh_set_section_writableflag(Elf32_Shdr *s, char f)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  if (f)
    s->sh_flags |= SHF_WRITE;
  else
    s->sh_flags &= (~SHF_WRITE);
  return (0);
}



/* SET/GET the executable flag */
char	elfsh_get_section_execflag(Elf32_Shdr *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return ((s->sh_flags & SHF_EXECINSTR));
}

char	elfsh_set_section_execflag(Elf32_Shdr *s, char f)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  if (f)
    s->sh_flags |= SHF_EXECINSTR;
  else
    s->sh_flags &= (~SHF_EXECINSTR);
  return (0);
}


/* SET/GET the mergeable flag */
char	elfsh_get_section_mergeableflag(Elf32_Shdr *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return ((s->sh_flags & SHF_MERGE));
}

char	elfsh_set_section_mergeableflag(Elf32_Shdr *s, char f)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  if (f)
    s->sh_flags |= SHF_MERGE;
  else
    s->sh_flags &= (~SHF_MERGE);
  return (0);
}


/* SET/GET the string flag */
char	elfsh_get_section_strflag(Elf32_Shdr *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return ((s->sh_flags & SHF_STRINGS));
}

char	elfsh_set_section_strflag(Elf32_Shdr *s, char f)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  if (f)
    s->sh_flags |= SHF_STRINGS;
  else
    s->sh_flags &= (~SHF_STRINGS);
  return (0);
}


/* SET/GET the link flag */
char	elfsh_get_section_linkflag(Elf32_Shdr *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return ((s->sh_flags & SHF_INFO_LINK));
}

char	elfsh_set_section_linkflag(Elf32_Shdr *s, char f)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  if (f)
    s->sh_flags |= SHF_INFO_LINK;
  else
    s->sh_flags &= (~SHF_INFO_LINK);
  return (0);
}


/* SET/GET the order flag */
char	elfsh_get_section_orderflag(Elf32_Shdr *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return ((s->sh_flags & SHF_LINK_ORDER));
}

char	elfsh_set_section_orderflag(Elf32_Shdr *s, char f)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  if (f)
    s->sh_flags |= SHF_LINK_ORDER;
  else
    s->sh_flags &= (~SHF_LINK_ORDER);
  return (0);
}





/* All the next functions are dumb read/write field access routine */
Elf32_Word		elfsh_get_section_info(Elf32_Shdr *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return (s->sh_info);
}

int		elfsh_set_section_info(Elf32_Shdr *s, Elf32_Word info)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  s->sh_info = info;
  return (0);
}

Elf32_Word		elfsh_get_section_entsize(Elf32_Shdr *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return (s->sh_entsize);
}

int		elfsh_set_section_entsize(Elf32_Shdr *s, Elf32_Word entsize)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  s->sh_entsize = entsize;
  return (0);
}

Elf32_Word	elfsh_get_section_link(Elf32_Shdr *s)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return (s->sh_link);
}

int		elfsh_set_section_link(Elf32_Shdr *s, Elf32_Word link)
{
  if (!s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  s->sh_link = link;
  return (0);
}




Elf32_Off	elfsh_get_section_foffset(Elf32_Shdr *s)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return (s->sh_offset);
}

int	elfsh_set_section_foffset(Elf32_Shdr *s, Elf32_Off offset)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  s->sh_offset = offset;
  return (0);
}

Elf32_Addr	elfsh_get_section_addr(Elf32_Shdr *s)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return (s->sh_addr);
}

int	elfsh_set_section_addr(Elf32_Shdr *s, Elf32_Addr addr)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  s->sh_addr = addr;
  return (0);
}


Elf32_Word	elfsh_get_section_align(Elf32_Shdr *s)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return (s->sh_addralign);
}

int	elfsh_set_section_align(Elf32_Shdr *s, Elf32_Word align)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  s->sh_addralign = align;
  return (0);
}

Elf32_Word	elfsh_get_section_size(Elf32_Shdr *s)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return (s->sh_size);
}

int	elfsh_set_section_size(Elf32_Shdr *s, Elf32_Word size)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  s->sh_size = size;
  return (0);
}

Elf32_Word	elfsh_get_section_type(Elf32_Shdr *s)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  return (s->sh_type);
}

int	elfsh_set_section_type(Elf32_Shdr *s, Elf32_Word type)
{
  if (NULL == s)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter \n", -1);
  s->sh_type = type;
  return (0);
}



/*
**
**
** ELFsh handlers
**
** These functions are usually not used by something else than ELFsh
**
**
*/


/* Get SHT entry by index */
Elf32_Shdr		*elfsh_get_sht_entry_by_index(Elf32_Shdr *s, u_int index)
{
  return (s + index);
}


/* Get SHT entry by name */
Elf32_Shdr		*elfsh_get_sht_entry_by_name(elfshobj_t *file, char *name)
{
  elfshsect_t		*sect;
  u_int			index;
  char			*curnam;

  /* Sanity check */
  if (file == NULL || name == NULL)
    ELFSH_SETERROR("[elfsh:get_sht_entry_by_name] Invalid NULL parameter\n",
		   NULL);
  else if (file->sectlist == NULL && elfsh_read_obj(file) < 0)
    return (NULL);

  /* Find SHT entry */
  for (sect = file->sectlist, index = 0; sect != NULL; sect = sect->next)
    {
      curnam = elfsh_get_section_name(file, sect);
      if (curnam != NULL && !strcmp(curnam, name))
	return (sect->shdr);
    }

  /* Not found */
  ELFSH_SETERROR("[elfsh:get_sht_entry_by_name] Cannot find SHT entry\n",
		 NULL);
}
