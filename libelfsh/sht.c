/**
 * @file sht.c
 * @ingroup libelfsh
** sht.c for libelfsh
** 
** Started on  Mon Feb 26 04:16:18 2001 jfv
**
** $Id: sht.c,v 1.9 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"

/**
 * @brief Sort SHT by file offset on ET_REL objects
 * Mandatory on gcc 2.95.2/2.96 generated bins, maybe others 
 * @param file
 * @return
 */
int			elfsh_sort_sht(elfshobj_t *file)
{
  elfshsect_t		*actual;
  elfshsect_t		*tmpsct;
  elfshsect_t		*next;
  elfsh_Shdr		tmp;
  unsigned int			index;
  unsigned int			pass;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  else if (file->sht == NULL && NULL == elfsh_get_sht(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get SHT", -1);

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
	  elfsh_print_sectlist(file, "sort_sht");
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

		  actual->prev->next = actual->next;
		  actual->next->prev = actual->prev;
		  tmpsct = actual->next->next;
		  actual->next->next = actual;
		  actual->prev = actual->next;
		  actual->next = tmpsct;
		  if (tmpsct)
		    tmpsct->prev = actual;

		  /* this is buggy, spot the difference ... 
		     tmpsct = actual->prev;
		     actual->prev->next = actual->prev = actual->next;
		     actual->next->prev = tmpsct;
		     if (actual->next->next != NULL)
		     actual->next->next->prev = actual;
		     tmpsct = actual->next->next;
		     actual->next->next = actual;
		     actual->next = tmpsct;
		  */

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
  elfsh_print_sectlist(file, "final_state");
  puts("-------------------------------------------");
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Synchronize duplicated names for sections 
 * @param file
 * @return
 */
void		elfsh_sync_sectnames(elfshobj_t *file)
{
  elfshsect_t	*s;
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (s = file->sectlist; s != NULL; s = s->next)
    {
      name = elfsh_get_section_name(file, s);
      if (s->name != NULL)
	XFREE(__FILE__, __FUNCTION__, __LINE__,s->name);
      s->name = (name != NULL ? strdup(name) : NULL);
 
    }
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * @brief Synchronize SHT (mostly used when removing sections) 
 * @param file
 * @return
 */
void		elfsh_sync_sht(elfshobj_t *file)
{
  unsigned int		idx;
  elfshsect_t	*cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (cur = file->sectlist, idx = 0; idx < file->hdr->e_shnum; idx++)
    {
      cur->shdr = file->sht + idx;
      cur->index = idx;
      cur = cur->next;
    }
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/**
 * Return a pointer on the runtime SHT 
 * @param file
 * @param num
 * @return
 */
void		*elfsh_get_runtime_sht(elfshobj_t *file, int *num)
{
  elfshsect_t	*rshstrtab;
  int		ret;
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Sanity checks */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid NULL parameter", NULL);

  /* Build RSHT if inexistant */
  if (file->rsht == NULL)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__,file->rsht, file->hdr->e_shentsize, NULL);	
      rshstrtab = elfsh_create_section(ELFSH_SECTION_NAME_RSHSTRTAB);
      file->rhdr.rshtnbr = 1;
      ret = strlen(ELFSH_SECTION_NAME_RSHSTRTAB) + 1;
      file->rsht[0].sh_size = ret;
      XALLOC(__FILE__, __FUNCTION__, __LINE__,str, ret, NULL);
      memcpy(str, ELFSH_SECTION_NAME_RSHSTRTAB, ret);
      ret = elfsh_add_runtime_section(file, rshstrtab, 0, str);
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot add runtime sections strings table", NULL);
      file->secthash[ELFSH_SECTION_RSHSTRTAB] = rshstrtab;
      rshstrtab->curend = file->rsht[0].sh_size;
    }


  /* Everything was OK */
  if (num != NULL)
    *num = file->rhdr.rshtnbr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->rsht));
}


/**
 * @brief Return a ptr on the section header table 
 * @param file
 * @param num
 * @return
 */
void		*elfsh_get_sht(elfshobj_t *file, int *num)
{
  elfshsect_t	*s;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  if (file->sht != NULL)
    goto end;

  /* Load the section header table and allocate every elfshsect_t */
  else if (elfsh_load_sht(file) < 0 && elfsh_rebuild_sht(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cant load SHT", NULL);
  
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
  elfsh_sync_sectnames(file);

  /* Some final stuffs */
 end:
  if (num != NULL)
    *num = file->hdr->e_shnum;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->sht));
}


/**
 * Change endianess of SHT 
 * @param s
 * @param byteorder
 * @param shnum
 * @return
 */
int		elfsh_endianize_sht(elfsh_Shdr *s, 
				    char byteorder, 
				    uint16_t shnum)			    
{
  int		i;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  /* Deal with cross-endianness binaries */
#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (byteorder == ELFDATA2MSB) {
#elif __BYTE_ORDER == __BIG_ENDIAN
  if (byteorder == ELFDATA2LSB) {
#else
#error Unexpected __BYTE_ORDER !
#endif
    for (i = 0; i < shnum; i++)
      {
	s->sh_name      = swap32(s->sh_name);
	s->sh_type      = swap32(s->sh_type);
	s->sh_flags     = swaplong(s->sh_flags);
	s->sh_addr      = swaplong(s->sh_addr);
	s->sh_offset    = swaplong(s->sh_offset);
	s->sh_size      = swaplong(s->sh_size);
	s->sh_link      = swap32(s->sh_link);
	s->sh_info      = swap32(s->sh_info);
	s->sh_addralign = swaplong(s->sh_addralign);
	s->sh_entsize   = swaplong(s->sh_entsize);
	s++;
      }
  }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return an array of section header 
 * @param file
 * @return
 */
int		elfsh_load_sht(elfshobj_t *file)
{
  elfshsect_t	*new;
  int		size;
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  else if (file->sht != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Read table */
  size = file->hdr->e_shnum * file->hdr->e_shentsize;
  if (!file->hdr->e_shoff)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "SHT file offset is NULL", -1);

  if (file->hdr->e_shoff > file->fstat.st_size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "SHT file offset is larger than the file itself", -1);

  XSEEK(file->fd, file->hdr->e_shoff, SEEK_SET, -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,file->sht, size, -1);
  XREAD(file->fd, file->sht, size, -1);
  
  elfsh_endianize_sht(file->sht,
		      file->hdr->e_ident[EI_DATA], 
		      file->hdr->e_shnum);

  /*
  ** Allocate a elfshsect_t per section and put it
  ** in the file->sectlist double linked list
  */
  for (index = 0; index < file->hdr->e_shnum; index++)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(elfshsect_t), -1);
      size = elfsh_add_section(file, new, index, NULL,
			       ELFSH_SHIFTING_COMPLETE);
      if (size < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to add section", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Retreive the section header giving the section symbol from .symtab 
 * @param file
 * @param sym
 * @return
 */
elfsh_Shdr		*elfsh_get_shtentry_from_sym(elfshobj_t *file,
						     elfsh_Sym	*sym)
{
  elfshsect_t		*sect;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  sect = elfsh_get_section_from_sym(file, sym);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (sect == NULL ? NULL : sect->shdr));
}

/**
 * Retreive the symbol for the section giving the section header
 * @param file
 * @param hdr
 * @return
 */
elfsh_Sym		*elfsh_get_sym_from_shtentry(elfshobj_t *file,
						     elfsh_Shdr *hdr)
{
  elfsh_Sym		*current;
  int			index;
  unsigned int			nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || hdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  else if (file->sht == NULL && NULL == elfsh_get_sht(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SHT", NULL);

  /* Load symtab if necessary */
  else if (file->secthash[ELFSH_SECTION_SYMTAB] == NULL &&
	   NULL == elfsh_get_symtab(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SYMTAB", NULL);

  else if (!file->secthash[ELFSH_SECTION_SYMTAB]->shdr->sh_size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Symtab is void", NULL);
  nbr = file->secthash[ELFSH_SECTION_SYMTAB]->shdr->sh_size / 
    sizeof(elfsh_Sym);

  /* Find the wanted symbol */
  index = 0;
  for (current = file->secthash[ELFSH_SECTION_SYMTAB]->data;
       index < nbr; index++)
    if (elfsh_get_symbol_type(current + index) == STT_SECTION &&
	current[index].st_value == hdr->sh_addr)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (current + index));

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Symbol not found", NULL);
}

/**
 * Create a section header 
 *
 * @param name
 * @param type
 * @param flags
 * @param addr
 * @param offset
 * @param size
 * @param link
 * @param info
 * @param align
 * @param entsize
 * @return
 */
elfsh_Shdr		elfsh_create_shdr(elfsh_Word name,
					  elfsh_Word type,
					  elfsh_Word flags,
					  eresi_Addr addr,
					  eresi_Off offset,
					  elfsh_Word size,
					  elfsh_Word link,
					  elfsh_Word info,
					  elfsh_Word align,
					  elfsh_Word entsize)
{
  elfsh_Shdr		new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/**
 * Add a section header to the object 
 * @param file
 * @param hdr
 * @param range
 * @param name
 * @param shiftflag
 * @return
 */
int		elfsh_insert_shdr(elfshobj_t	*file,
				  elfsh_Shdr	hdr,
				  unsigned int		range,
				  char		*name,
				  char		shiftflag)
{
  elfshsect_t	*tmp;
  int		index;
  int		shname;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  // Debug to remove
  //printf("Injecting SHDR for section %s \n", name);
  //fflush(stdout);

  /* Preliminary checks */
  if (file == NULL ||
      (file->hdr == NULL && elfsh_get_hdr(file) == NULL) ||
      (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get ELF header or SHT", -1);

  /* Fixup the range */
  if (range == ELFSH_SECTION_LAST)
    range = file->hdr->e_shnum;
  else if (range > file->hdr->e_shnum)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid range for injection", -1);

  /* Insert the name string in .shstrtab */
  shname = elfsh_insert_in_shstrtab(file, name);
  if (shname < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot inject data in .shstrtab", -1);
  hdr.sh_name = (unsigned int) shname;

  /* Keep track of what is shifted since our new shdr is not
     yet in SHT */
  if (range > file->secthash[ELFSH_SECTION_SHSTRTAB]->index)
    hdr.sh_offset += strlen(name) + 1;

  /* Extend the SHT and fill the new slot */
  XREALLOC(__FILE__, __FUNCTION__, __LINE__,file->sht, file->sht,
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
  if (hdr.sh_offset <= file->hdr->e_shoff && shiftflag)
    file->hdr->e_shoff += hdr.sh_size;

  /* Update the .shstrtab index in the ELF header if necessary */
  if (range <= file->hdr->e_shstrndx)
    file->hdr->e_shstrndx++;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (range));
}

/**
 * Add a section header to the object 
 * @param file
 * @param hdr
 * @param range
 * @param name
 * @param shiftflag
 * @return
 */
int		elfsh_insert_runtime_shdr(elfshobj_t	*file,
					  elfsh_Shdr	hdr,
					  unsigned int		range,
					  char		*name,
					  char		shiftflag)
{
  elfshsect_t	*tmp;
  int		index;
  int		shname;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_RUNTIME__
  printf("[DEBUG_RUNTIME] Asking runtime shdr injection on range %d \n", range);
#endif

  /* Preliminary checks */
  if (file == NULL ||
      (file->hdr == NULL && elfsh_get_hdr(file) == NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get ELF header or SHT", -1);

  if (!file->rsht)
    range++;

  /* Runtime sections check */
  if (elfsh_get_runtime_sht(file, NULL) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get runtime SHT", -1);

  /* Fixup the range */
  if (range == ELFSH_SECTION_LAST)
    range = file->rhdr.rshtnbr;
  else if (range > file->rhdr.rshtnbr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid range for injection", -1);

  /* Insert the name string in .rshstrtab */
  shname = elfsh_insert_in_rshstrtab(file, name);
  if (shname < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot inject data in .rshstrtab", -1);
  hdr.sh_name = (unsigned int) shname;

  /* Extend the SHT and fill the new slot */
  XREALLOC(__FILE__, __FUNCTION__, __LINE__,file->rsht, file->rsht,
	   file->hdr->e_shentsize * (file->rhdr.rshtnbr + 1), -1);
  
  if (range != file->rhdr.rshtnbr)
    memmove(file->rsht + range + 1,
	    file->rsht + range,
	    (file->rhdr.rshtnbr - range) * file->hdr->e_shentsize);

  file->rsht[range] = hdr;
  file->rhdr.rshtnbr++;


#if __DEBUG_RUNTIME__
  printf("[DEBUG_RUNTIME] Runtime shdr injected on range %d \n", range);
#endif

  /* Update foffset for all section header whoose section is placed after SHT */
  for (tmp = file->rsectlist, index = 0; 
       tmp && index < file->rhdr.rshtnbr; 
       index++)
    {

      /*
      ** Since we reallocated the rsht, we have to update the shdr pointer
      ** since its pointing to the old chunk !
      ** Pass headers without body (this include the just inserted one)
      */
      if (index != range)
	{
	  tmp->shdr = file->rsht + index;
	  tmp = tmp->next;
	}
      else 
	tmp->name = (char *) file->secthash[ELFSH_SECTION_RSHSTRTAB] + tmp->shdr->sh_name;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (range));
}

/**
 * Get the section's name in .shstrtab
 * @param file
 * @param s
 * @return
 */
char		*elfsh_get_section_name(elfshobj_t *file,
					elfshsect_t *s)
{
  volatile char	*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity check */
  if (file == NULL || s == NULL ||
      file->secthash[ELFSH_SECTION_SHSTRTAB] == NULL ||
      (file->sht == NULL && NULL == elfsh_get_sht(file, NULL)))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SHSTRTAB", NULL);

  /* Additional check for runtime sections */
  if (elfsh_section_is_runtime(s) && 
      (!file->rsectlist || ! file->rsht || 
       !file->secthash[ELFSH_SECTION_RSHSTRTAB]))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,                            
                      "Unable to get runtime sections information", NULL); 

  /* Return section's name */
  if (elfsh_section_is_runtime(s))
    str = file->secthash[ELFSH_SECTION_RSHSTRTAB]->data;
  else
    str = file->secthash[ELFSH_SECTION_SHSTRTAB]->data;

  /* NULL str check added for supporting binaries without shstrrtab */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (str == NULL ? NULL : (char *) str + s->shdr->sh_name));
}

/**
 * Set the section's name in .shstrtab 
 * @param file
 * @param s
 * @param name
 * @return
 */
int			elfsh_set_section_name(elfshobj_t	*file,
					       elfshsect_t	*s,
					       char		*name)
{
  char		*str;
  unsigned int		len;
  unsigned int		new_len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity check */
  if (file == NULL || s == NULL || name == NULL ||
      file->secthash[ELFSH_SECTION_SHSTRTAB] == NULL ||
      (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get SHSTRTAB", -1);

  /* Additional check for runtime sections */
  if (elfsh_section_is_runtime(s) && 
      (!file->rsectlist || ! file->rsht || 
       !file->secthash[ELFSH_SECTION_RSHSTRTAB]))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
                      "Unable to get runtime sections", NULL); 

  /* Return section's name */
  if (!elfsh_section_is_runtime(s))
    str = file->secthash[ELFSH_SECTION_SHSTRTAB]->data;
  else
    str = file->secthash[ELFSH_SECTION_RSHSTRTAB]->data;
  str += s->shdr->sh_name;
  len = strlen(str);
  new_len = strlen(name);

  /* Do not allocate new place if possible */
  if (len >= new_len)
    strncpy(str, name, new_len);

  /* Append the name to the section string table */
  else if (!elfsh_section_is_runtime(s))
    s->shdr->sh_name = elfsh_insert_in_shstrtab(file, name);

  /* Append t he name to the runtime section string table */
  else
    s->shdr->sh_name = elfsh_insert_in_rshstrtab(file, name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->shdr->sh_name));
}

/**
 * Tell elfsh that we dont want the sht in the output file 
 * @param file
 */
void		elfsh_remove_sht(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  file->shtrm = 1;

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * SET/GET the allocatable flag 
 * @param s
 * @return
 */
char	elfsh_get_section_allocflag(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((s->sh_flags & SHF_ALLOC)));
}

/**
 * Set the allocatable flag
 * @param s
 * @param f
 * @return
 */
char	elfsh_set_section_allocflag(elfsh_Shdr *s, eresi_Addr f)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  if (f)
    s->sh_flags |= SHF_ALLOC;
  else
    s->sh_flags &= (~SHF_ALLOC);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * SET/GET the writable flag 
 * @param s
 * @return
 */
char	elfsh_get_section_writableflag(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((s->sh_flags & SHF_WRITE)));
}

/**
 * SET writableflag
 * @param s
 * @param f
 * @return
 */
char	elfsh_set_section_writableflag(elfsh_Shdr *s, eresi_Addr f)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter ", -1);
  if (f)
    s->sh_flags |= SHF_WRITE;
  else
    s->sh_flags &= (~SHF_WRITE);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * SET/GET the executable flag 
 * @param s
 * @return
 */
char	elfsh_get_section_execflag(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((s->sh_flags & SHF_EXECINSTR)));
}

/**
 * SET executable flag
 * @param s
 * @param f
 * @return
 */
char	elfsh_set_section_execflag(elfsh_Shdr *s, eresi_Addr f)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  if (f)
    s->sh_flags |= SHF_EXECINSTR;
  else
    s->sh_flags &= (~SHF_EXECINSTR);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * SET/GET the mergeable flag 
 * @param s
 * @return
 */
char	elfsh_get_section_mergeableflag(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((s->sh_flags & SHF_MERGE)));
}

/**
 * SET the mergeable flag
 * @param s
 * @param f
 * @return
 */
char	elfsh_set_section_mergeableflag(elfsh_Shdr *s, eresi_Addr f)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter ", -1);
  if (f)
    s->sh_flags |= SHF_MERGE;
  else
    s->sh_flags &= (~SHF_MERGE);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * SET/GET the string flag 
 * @param s
 * @return
 */
char	elfsh_get_section_strflag(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((s->sh_flags & SHF_STRINGS)));
}

/**
 * SET the string flag
 * @param s
 * @param f
 * @return
 */
char	elfsh_set_section_strflag(elfsh_Shdr *s, eresi_Addr f)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter ", -1);
  if (f)
    s->sh_flags |= SHF_STRINGS;
  else
    s->sh_flags &= (~SHF_STRINGS);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * SET/GET the link flag 
 * @param s
 * @return
 */
char	elfsh_get_section_linkflag(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((s->sh_flags & SHF_INFO_LINK)));
}

/**
 * SET the link flag
 * @param s
 * @param f
 * @return
 */

char	elfsh_set_section_linkflag(elfsh_Shdr *s, eresi_Addr f)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter ", -1);
  if (f)
    s->sh_flags |= SHF_INFO_LINK;
  else
    s->sh_flags &= (~SHF_INFO_LINK);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * SET/GET the order flag 
 * @param s
 * @return
 */
char	elfsh_get_section_orderflag(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((s->sh_flags & SHF_LINK_ORDER)));
}

/**
 * SET the order flag
 * @param s
 * @param f
 * @return
 */

char	elfsh_set_section_orderflag(elfsh_Shdr *s, eresi_Addr f)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  if (f)
    s->sh_flags |= SHF_LINK_ORDER;
  else
    s->sh_flags &= (~SHF_LINK_ORDER);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * All the next functions are dumb read/write field access routine 
 * @param s
 * @return
 */
elfsh_Word		elfsh_get_section_info(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->sh_info));
}

/**
 * Set section info
 * @param s
 * @param info
 * @return
 */
int		elfsh_set_section_info(elfsh_Shdr *s, eresi_Addr info)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  s->sh_info = (elfsh_Word) info;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * get section entsize
 * @param s Pointer to section structure.
 * @return Return section entry size or -1 on error.
 */
elfsh_Word		elfsh_get_section_entsize(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->sh_entsize));
}

/**
 * set section entsize
 * @param s
 * @param entsize
 * @return
 */
int		elfsh_set_section_entsize(elfsh_Shdr *s, eresi_Addr entsize)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  s->sh_entsize = (elfsh_Word) entsize;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * get section link
 * @param s
 * @return
 */
elfsh_Word	elfsh_get_section_link(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->sh_link));
}

/**
 * set section link
 * @param s
 * @param link
 * @return
 */
int		elfsh_set_section_link(elfsh_Shdr *s, eresi_Addr link)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  s->sh_link = (elfsh_Word) link;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * get section foffset
 * @param s
 * @return
 */
eresi_Off	elfsh_get_section_foffset(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->sh_offset));
}

/**
 * set section foffset
 * @param s
 * @param offset
 * @return
 */
int	elfsh_set_section_foffset(elfsh_Shdr *s, eresi_Addr offset)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  s->sh_offset = (eresi_Off) offset;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * get section addr
 * @param s Pointer to section structure.
 * @return Return section address or -1 on error.
 */
eresi_Addr	elfsh_get_section_addr(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->sh_addr));
}

/**
 * set section addr
 * @param s Pointer to section structure.
 * @param addr New address of section
 * @return Returns 0 on success or -1 on error.
 */
int	elfsh_set_section_addr(elfsh_Shdr *s, eresi_Addr addr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  s->sh_addr = addr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * get section align
 * @param s
 * @return
 */
elfsh_Word	elfsh_get_section_align(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->sh_addralign));
}

/**
 * set section align
 * @param s
 * @param align
 * @return
 */
int	elfsh_set_section_align(elfsh_Shdr *s, eresi_Addr align)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  s->sh_addralign = (elfsh_Word) align;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * get section size
 * @param s
 * @return
 */
elfsh_Word	elfsh_get_section_size(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->sh_size));
}

/**
 * set section size
 * @param s Pointer to section structure.
 * @param size New size of section.
 * @return Returns 0 on success or -1 on error.
 */
int	elfsh_set_section_size(elfsh_Shdr *s, eresi_Addr size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter ", -1);
  s->sh_size = (elfsh_Word) size;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * get section type
 * @param s
 * @return Return section type or -1 on error.
 */
elfsh_Word	elfsh_get_section_type(elfsh_Shdr *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->sh_type));
}

/**
 * set section type
 * @param s
 * @param type
 * @return Returns 0 on success or -1 on error.
 */
int	elfsh_set_section_type(elfsh_Shdr *s, eresi_Addr type)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter ", -1);
  s->sh_type = (elfsh_Word) type;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * ELFsh handlers
 *
 * These functions are usually not used by something else than ELFsh
 *
 */

/**
 * Get SHT or runtime SHT entry by index
 * @param s
 * @param index
 * @return Pointer to a section structure.
 */
elfsh_Shdr		*elfsh_get_sht_entry_by_index(elfsh_Shdr *s, 
						      eresi_Addr index)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s + index));
}

/**
 * Get SHT entry by name 
 * @param file
 * @param name
 * @return
 */
elfsh_Shdr		*elfsh_get_sht_entry_by_name(elfshobj_t *file, char *name)
{
  elfshsect_t		*sect;
  unsigned int			index;
  char			*curnam;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity check */
  if (file == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  else if (file->sectlist == NULL && elfsh_read_obj(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to read object", NULL);

  /* Find SHT entry */
  for (sect = file->sectlist, index = 0; sect != NULL; sect = sect->next)
    {
      curnam = elfsh_get_section_name(file, sect);
      if (curnam != NULL && !strcmp(curnam, name))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->shdr));
    }

  /* Find runtime SHT entry */
  for (sect = file->rsectlist, index = 0; sect != NULL; sect = sect->next)
    {
      curnam = elfsh_get_section_name(file, sect);
      if (curnam != NULL && !strcmp(curnam, name))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->shdr));
    }

  /* Not found */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Cannot find SHT entry", NULL);
}
