/**
 * @file section.c
 * @ingroup libelfsh
** section.c for libelfsh
** 
** @brief All handlers and lowlevel routines for sections management.
** 
** Started on  Mon Feb 26 04:12:42 2001 jfv
** $Id: section.c,v 1.19 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"

/**
 * @brief Just used to avoid code redundancy in elfsh_add_section()
 * @param sct
 * @param tmp
 * @param mode
 * @return
 */
char	elfsh_shift_section(elfshsect_t *sct, elfshsect_t *tmp, unsigned char mode)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Used by SHT reconstruction engine */
  if (mode >= ELFSH_SHIFTING_NONE &&
      tmp->shdr->sh_link && (sct->index <= tmp->shdr->sh_link))
    tmp->shdr->sh_link++;
  
  /* Used by MIPS/PLT reconstruction */
  /* No shifting except an incrementation of index for .plt */
  if (mode >= ELFSH_SHIFTING_MIPSPLT)
    tmp->index++;
  
  /* File perspective section shifting */
  if (mode >= ELFSH_SHIFTING_PARTIAL)
    tmp->shdr->sh_offset += sct->shdr->sh_size;

  /* Loader perspective mapped section shifting */
  if (mode >= ELFSH_SHIFTING_COMPLETE && tmp->shdr->sh_addr)
      tmp->shdr->sh_addr += sct->shdr->sh_size;

  /* Return 1 */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}


/**
 * INTERNAL FUNCTION : Add a given section to the file's section list
 *
 * SHOULD -NOT- manipulation symbol table because we are called from
 * elfsh_load_sht() which has not loaded symtab yet.
 *
 * This function is used for constructing the map (elfshobj_t) of a 
 * binary file in memory. It is called as well at each new section
 * injection.
 *
 * See libelfsh/inject.c for the user-friendly section injection API
 *
 * @param file
 * @param sct
 * @param range
 * @param dat
 * @param shiftmode
 * @return
 */
int		elfsh_add_section(elfshobj_t	*file,
				  elfshsect_t	*sct,
				  unsigned int		range,
				  void		*dat,
				  int		shiftmode)
{
  elfshsect_t	*tmp;
  char		inserted;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  
#if __DEBUG_RUNTIME__
  printf("[DEBUG_RUNTIME] Adding section with range = %u, file->rhdr.rshtnbr = %u, shnum = %u (%s) \n", 
	 range, file->rhdr.rshtnbr, file->hdr->e_shnum, sct->name);
#endif

  /* Load the ELF header if not done */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (file->sht == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "SHT not loaded", -1);

  else if (elfsh_section_is_runtime(sct) && !file->rsht)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "RSHT not loaded", -1);

  /* Refuse to insert a section for some conditions */
  if ((elfsh_section_is_runtime(sct) && range >= file->rhdr.rshtnbr) ||
      range >= file->hdr->e_shnum)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown requested section slot", -1);

  else if (sct->flags & ELFSH_SECTION_INSERTED)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Already inserted", -1);
  else if (sct->data != NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section is not empty", -1);

  sct->shdr   = (elfsh_section_is_runtime(sct) ? file->rsht : file->sht) + range;

#if __DEBUG_RUNTIME__
  printf("[DEBUG_RUNTIME] Added section address = %08X \n", sct->shdr->sh_addr);
#endif

  sct->parent = file;
  sct->index  = range;
  sct->data   = dat;
  inserted    = 0;
  
  /* We cannot print the name here because some sects do not have one yet */
#if __DEBUG_SECTS__ 
  printf("[DEBUG:add_section] Adding section at index = %u from file %s \n",
	 sct->index, file->name);
#endif

  /* Parse the section list */
  for (tmp = elfsh_section_is_runtime(sct) ? file->rsectlist : file->sectlist;
       tmp != NULL && tmp->next != NULL; tmp = tmp->next)
    if (tmp->index == range)
      {
	/* Insert the new section */
	sct->prev = tmp->prev;
	sct->next = tmp;
	if (sct->prev != NULL)
	  sct->prev->next = sct;
	else if (elfsh_section_is_runtime(sct))
	  file->rsectlist = sct;
	else
	  file->sectlist = sct;

	tmp->prev = sct;

	/* Update indexes, file offset, vaddr */
	inserted = elfsh_shift_section(sct, tmp, shiftmode);
      }
    else if (tmp->index >= range)
      inserted = elfsh_shift_section(sct, tmp, shiftmode);

  /* ==> Now the special cases <== */

  /* The section is the first one inserted */
  if (!inserted && !tmp)
    {
      if (!elfsh_section_is_runtime(sct))
	{
	  file->sectlist = sct;
	  file->sectlist->prev = sct;
	}
      else
	{
	  file->rsectlist = sct;      
	  file->rsectlist->prev = sct;                                  
	}
    }

  /* The section must be inserted at the last place */
  else if (!inserted && !tmp->next)
    {
      tmp->next = sct;
      sct->prev = tmp;
      if (!elfsh_section_is_runtime(sct))
	file->sectlist->prev = sct;
      else
	file->rsectlist->prev = sct; 
    }

  /* shift the last section if the insertion has already been done */
  else if (inserted && !tmp->next)
    elfsh_shift_section(sct, tmp, shiftmode);

  /* Final fixes */
  sct->phdr   = elfsh_get_parent_segment(file, sct);
  sct->flags |= ELFSH_SECTION_INSERTED;
  sct->curend = sct->shdr->sh_size;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * @brief Add a runtime section. This function is internal, do not use it directly.
 * @param file The host file to add a section to.
 * @param sct The section descriptor to add.
 * @param range The index where to add the section.
 * @param dat The data to put in the section.
 * @return Success (0) or Error (-1).
 */
int             elfsh_add_runtime_section(elfshobj_t    *file,
					  elfshsect_t   *sct,
					  unsigned int         range,
					  void          *dat)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We do not shift any file offset in runtime injection */
  sct->flags |= ELFSH_SECTION_RUNTIME;
  ret = elfsh_add_section(file, sct, range, dat, ELFSH_SHIFTING_NONE);
  if (!ret)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot add runtime section", ret);
}

/**
 * @brief Internal function 
 * @param file
 * @param name
 * @param idx
 * @param strindex
 * @param num
 * @param sectlist
 * @return
 */
static 
elfshsect_t	*elfsh_get_section_by_name_withlist(elfshobj_t   *file,
						    char         *name,
						    int          *idx,
						    int          *strindex,
						    int          *num,
						    elfshsect_t	 *sectlist)   
{
  elfshsect_t	*section;
  char		*sname;
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0, section = sectlist;
       section;
       section = section->next, index++)
    {
      sname = elfsh_get_section_name(file, section);
      if (sname == NULL)
	continue;
      if (!strcmp(name, sname))
	{
	  if (idx != NULL)
	    *idx = index;
	  if (num != NULL)
	    *num = (section->curend ? 
		    section->curend : section->shdr->sh_size);
	  if (strindex != NULL)
	    *strindex = section->shdr->sh_link;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (section));
	}
    }
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Section not found", NULL);
}

/**
 * @brief Return a pointer on a section giving its name 
 * @param file
 * @param name
 * @param idx
 * @param strindex
 * @param num
 * @return
 */
elfshsect_t	*elfsh_get_section_by_name(elfshobj_t	*file,
					   char		*name,
					   int		*idx,
					   int		*strindex,
					   int		*num)
{
  elfshsect_t	*sect;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);

  else if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No SHT", NULL);

  sect = elfsh_get_section_by_name_withlist(file, name, idx, strindex, num, 
					    file->sectlist);
  if (!sect && file->rsectlist)
    sect = elfsh_get_section_by_name_withlist(file, name, idx, strindex, 
					      num, file->rsectlist);

  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section not found", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect));
}



/**
 * Search section by type, with list parameter
 * This is an internal function it should not be used directly 
 * @param file
 * @param type
 * @param range
 * @param index
 * @param strindex
 * @param num
 * @param sectlist
 * @param shnum
 * @return
 */
static
elfshsect_t		*elfsh_get_section_by_type_withlist(elfshobj_t	*file,
							    unsigned int	type,
							    int		range,
							    int		*index,
							    int		*strindex,
							    int		*num,
							    elfshsect_t	*sectlist,
							    uint16_t	shnum)
{
  int			number;
  int			local_index;
  elfshsect_t		*section;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Parse the section list */
  for (section = sectlist, local_index = number = 0;
       section != NULL && local_index < shnum;
       local_index++, section = section->next)
    {
      
      if (local_index == shnum)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Section not found", NULL);
      
      if (section->shdr->sh_type == type && ++number > range)
	{
	  if (strindex != NULL)
	    *strindex = section->shdr->sh_link;
	  if (num != NULL)
	    *num = (section->curend ?
		    section->curend : section->shdr->sh_size);
	  if (index != NULL)
	    *index = local_index;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (section));
	}

    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Section not found ..", NULL);
}

/**
 * Return the section header for the 'range' occurence of a 'type typed section
 * Return NULL if failed 
 * @param file
 * @param type
 * @param range
 * @param index
 * @param strindex
 * @param num
 * @return
 */
elfshsect_t		*elfsh_get_section_by_type(elfshobj_t	*file,
						   unsigned int	type,
						   int		range,
						   int		*index,
						   int		*strindex,
						   int		*num)

{
  elfshsect_t		*section;
  int			nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
		      
  else if (elfsh_get_sht(file, &nbr) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive SHT", NULL);

  /* Parse the section lists */
  section = elfsh_get_section_by_type_withlist(file, type, range, index, strindex,
					       num, file->sectlist, nbr);
  if (!section && file->rsectlist)
    section = elfsh_get_section_by_type_withlist(file, type, range, index, 
						 strindex, num, file->sectlist,
						 nbr);

  /* Result or error */
  if (!section)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section remained unfound", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (section));  
}

/**
 * Return the section pointer giving the section index in the sht 
 * @param file
 * @param index
 * @param strindex
 * @param num
 * @return
 */
elfshsect_t		*elfsh_get_section_by_index(elfshobj_t	*file,
						    eresi_Addr	index,
						    int		*strindex,
						    int		*num)
{
  elfshsect_t		*section;
  int			local;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  else if (NULL == elfsh_get_sht(file, num))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive SHT", NULL);

  for (section = file->sectlist, local = 0;
       NULL != section && local < index; local++)
    section = section->next;

  if (section == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find section", NULL);

  if (strindex != NULL)
    *strindex = file->sht[index].sh_link;
  if (num != NULL)
    *num = (section->curend ?
	    section->curend : section->shdr->sh_size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (section));
}

/**
 * Same that index research but for runtime injected sections 
 * @param file
 * @param index
 * @param strindex
 * @param num
 * @return
 */
elfshsect_t		*elfsh_get_rsection_by_index(elfshobj_t	*file,
						    eresi_Addr	index,
						    int		*strindex,
						    int		*num)
{
  elfshsect_t		*section;
  int			local;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  else if (NULL == elfsh_get_runtime_sht(file, num))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive RSHT", NULL);

  for (section = file->rsectlist, local = 0;
       NULL != section && local < index; local++)
    section = section->next;

  if (section == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find runtime section", NULL);

  if (strindex != NULL)
    *strindex = file->rsht[index].sh_link;
  if (num != NULL)
    *num = (section->curend ?
	    section->curend : section->shdr->sh_size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (section));
}

/**
 * @brief Load the code of a section giving its header. This function should be called after 
 * a elfsh_get_section_by_{name,type} to load the code.
 * @param file Parent file.
 * @param shdr Section header indicating where to load the section data from.
 * @return Return a pointer on the loaded section descriptor (elfshsect_t) or NULL if error.
 */
void		*elfsh_load_section(elfshobj_t *file, elfsh_Shdr *shdr)
{
  void		*sct;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (shdr == NULL || !shdr->sh_size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot load NULL section", NULL);

  XSEEK(file->fd, shdr->sh_offset, SEEK_SET, NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,sct, shdr->sh_size, NULL);
  XREAD(file->fd, sct, shdr->sh_size, NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sct));
}

/**
 * @brief Load an anonymous (unknown content) section.
 * @param file The parent file.
 * @param sect The section to be loaded.
 * @return Return a pointer on the loaded section descriptor (elfshsect_t) or NULL if error.
 */
void		*elfsh_get_anonymous_section(elfshobj_t *file, elfshsect_t *sect)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Bad parameter checking */
  if (file == NULL || sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* If the section is already loaded */
  if (sect->data != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_raw(sect)));

  sect->data = elfsh_load_section(file, sect->shdr);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_raw(sect)));
}

/**
 * @brief Create a new orphelin section from its name.
 * @param name The name of the section to create.
 * @return The descriptor of the newly created function.
 */
elfshsect_t	*elfsh_create_section(char *name)
{
  elfshsect_t	*enew;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL name", NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,enew, sizeof(elfshsect_t), NULL);
  enew->name = strdup(name);
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (enew));
}

/**
 * @brief Return the parent section for its address.
 * @param file The parent file.
 * @param value The virtual address.
 * @param offset Will be filled to indicate the offset within the section we want to read to.
 * @return The section descriptor, or NULL if error.
 */
elfshsect_t	*elfsh_get_parent_section(elfshobj_t	*file,
					  eresi_Addr   	value,
					  elfsh_SAddr   *offset)
{
  elfshsect_t	*s;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);    

  if (NULL == file->sht && !elfsh_get_sht(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get SHT", NULL);

  /* ET_DYN objects have a relative addressing inside the ELF file */
  if (elfsh_is_debug_mode())
    value -= file->rhdr.base;

  /* Look in static sections */
  for (s = file->sectlist; s; s = s->next)
    if (INTERVAL(s->shdr->sh_addr, value,
		 (s->shdr->sh_addr + s->shdr->sh_size)))
      {
	if (offset)
	  *offset = value - s->shdr->sh_addr;
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
      }

  /* ... but if the sections are runtime injected, their address is absolute */
  if (elfsh_is_debug_mode())
    value += file->rhdr.base;

  /* Now look in runtime sections list */
  for (s = file->rsectlist; s; s = s->next)
    if (INTERVAL(s->shdr->sh_addr, value,
		 (s->shdr->sh_addr + s->shdr->sh_size)))
      {
	if (offset)
	  *offset = value - s->shdr->sh_addr;
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
      }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to get parent section", NULL);
}

/**
 * Return the parent section for this file offset, else NULL
 * This function is only revelant for ondisk sections 
 * @param file
 * @param foff
 * @param offset
 * @return
 */
elfshsect_t	*elfsh_get_parent_section_by_foffset(elfshobj_t *file,
						     unsigned int	foff,
						     elfsh_SAddr *offset)
{
  elfshsect_t	*s;
  char		i;
  unsigned int		bval;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || elfsh_get_sht(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get parent section", NULL);

  for (s = file->sectlist; s; s = s->next)
    {
      bval = (s->next != NULL ? s->next->shdr->sh_offset :
	      s->shdr->sh_offset + s->shdr->sh_size);
      i = INTERVAL(s->shdr->sh_offset, foff, bval);
      if (i && elfsh_get_section_type(s->shdr) != SHT_NOBITS)
	{
	  if (offset != NULL)
	    *offset = foff - s->shdr->sh_offset;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
	}
    }
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Parent section remained unfound", NULL);
}

/**
 * Retreive the section giving the section symbol from .symtab 
 * @param file
 * @param sym
 * @return
 */
elfshsect_t		*elfsh_get_section_from_sym(elfshobj_t *file, 
						    elfsh_Sym *sym)
{
  elfshsect_t		*current;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || sym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  if (NULL == elfsh_get_sht(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Cannot get SHT", NULL);

  /* Compare ondisk sections first */
  for (current = file->sectlist; current != NULL; current = current->next)
    if (current->shdr->sh_addr == sym->st_value)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (current));

  /* Compare runtime injected section is still unfound */
  for (current = file->rsectlist; current != NULL; current = current->next)
    if (current->shdr->sh_addr == sym->st_value)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (current));

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "No correspondance", NULL);
}

/**
 * This function appends a single char to a section by extending it 
 * @param sect
 * @param c
 * @param size
 * @return
 */
int		elfsh_fill_section(elfshsect_t	*sect, 
				   char		c,
				   unsigned int	size)
{
  char		*str;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  str = alloca(size);
  memset(str, c, size);
  ret = elfsh_append_data_to_section(sect, str, size);
  if (ret <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to append data to section", NULL);
  else
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* 
 * This function is quite context dependant and should be rewritten.
 * Symbol table shifting might be avoided in some obscure
 * not-yet-encountered cases. For now it works as requested quite
 * correctly.
 *
 * Note : Section placed before non-relocatable sections should
 * not use this function because all mapped following sections
 * virtual addresses will change. Be sure to have decent 
 * (recovered) relocation tables before using it on those sections.
 *
 * Only strong dataflow engine seems to be the solution for those.
 *
 * This function is not e2dbg safe and should only be used ondisk !
 *
 * @param sect
 * @param input
 * @param len
 * @return
 */
int		elfsh_append_data_to_section(elfshsect_t	*sect,
					     void		*input,
					     unsigned int		len)
{
  elfshsect_t	*actual;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Sanity checks */
  if (sect == NULL || input == NULL || !len)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  //printf("Extending %s section from %u bytes (curend = %u) \n", 
  //	 sect->name, len, sect->curend);
  //fflush(stdout);

  /* Put the data at the end of the section */
  /* XXX: need to re-mmap in the case of runtime section */
  if (sect->curend + len > sect->shdr->sh_size)
    {
      if (elfsh_section_is_runtime(sect) && sect->shdr->sh_addr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot extend a runtime injected zone", -1);

      if (sect->data != NULL)
	XREALLOC(__FILE__, __FUNCTION__, __LINE__,sect->data, sect->data, sect->shdr->sh_size + len, -1);
      else
	XALLOC(__FILE__, __FUNCTION__, __LINE__,sect->data, len, -1);
      
      memcpy(sect->data + sect->shdr->sh_size, input, len);
      sect->shdr->sh_size += len;
      sect->curend += len;
    }
  
  /* Enough room in the section to put our data */
  else
    {
      memcpy(sect->data + sect->curend, input, len);
      sect->curend += len;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->curend - len));
    }

  /* Avoid some unwanted shifting at command line or on runtime injected sects */
  if (sect->parent->rights == O_RDONLY ||
      elfsh_section_is_runtime(sect))
    goto end;
  
  /* Modify the section header table file offset if necessary */
  if (sect->parent->hdr->e_shoff >= sect->shdr->sh_offset)
    sect->parent->hdr->e_shoff += len;

  /* Update vaddr and foffset for all next sections */
  for (actual = sect; actual != NULL; actual = actual->next)
    if (actual != sect)
      {
	
	/*
	printf("Shifted section %s from %08X to %08X (foff : %u -> %u) \n", 
	       actual->name, actual->shdr->sh_addr, 
	       actual->shdr->sh_addr + len, 
	       actual->shdr->sh_offset, 
	       actual->shdr->sh_offset + len);
	fflush(stdout);
	*/
	
	/* Do not shift virtual address for mapped sections located
	   after an unmapped section */
	if (sect->shdr->sh_addr != NULL && actual->shdr->sh_addr != NULL)
	  actual->shdr->sh_addr += len;
	if (!elfsh_section_is_runtime(actual))
	  actual->shdr->sh_offset += len;
      }


 end:

  /* Synchronize SYMTAB and all its sorted instances */
  if (sect->shdr->sh_addr != NULL)
    {

#if __DEBUG_RELADD__
      printf("[DEBUG_RELADD] Shift symtab from append_data\n");
#endif

      elfsh_shift_symtab(sect->parent,
			 sect->shdr->sh_addr + sect->shdr->sh_size - len,
			 len);
    }

  /* Return section offset where the data has been inserted */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (sect->shdr->sh_size - len));
}

/**
 * Remove a section
 * XXX-runtime : This section should only be used for ondisk modifications 
 * @param obj Pointer to elfsh object.
 * @param name Section name
 * @return
 */
int			elfsh_remove_section(elfshobj_t *obj, char *name)
{
  elfshsect_t		*todel;
  elfshsect_t		*cur;
  elfsh_Shdr		shdr;
  elfsh_Shdr		*enew;
  elfsh_Phdr		*pht;
  elfsh_Sym		*sym;
  unsigned int			idx;
  unsigned int			size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (obj == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);

  todel = elfsh_get_section_by_name(obj, name, NULL, NULL, NULL);
  if (todel == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown section to remove", -1);

  if ((todel->prev && todel->prev->shdr->sh_addr) &&
      (todel->next && todel->next->shdr->sh_addr))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section not removable", -1);

  /* Unlink */
  if (todel->prev)
    todel->prev->next = todel->next;
  if (todel->next)
    todel->next->prev = todel->prev;

  /* Remove SHT header */
  shdr = *todel->shdr;
  size = (obj->hdr->e_shnum - todel->index - 1) * sizeof(elfsh_Shdr);
  if (size)
    memcpy(obj->sht + todel->index, obj->sht + todel->index + 1, size);
  obj->hdr->e_shnum--;
  XALLOC(__FILE__, __FUNCTION__, __LINE__,enew, obj->hdr->e_shnum * sizeof(elfsh_Shdr), -1);
  memcpy(enew, obj->sht, obj->hdr->e_shnum * sizeof(elfsh_Shdr));
  XFREE(__FILE__, __FUNCTION__, __LINE__,obj->sht);
  obj->sht = enew;
  elfsh_sync_sht(obj);
  elfsh_sync_sectnames(obj);

  /* shift sections file offsets and sh_link */
  for (cur = obj->sectlist; cur; cur = cur->next)
    {
      if (cur->shdr->sh_offset > shdr.sh_offset)
	cur->shdr->sh_offset -= shdr.sh_size;
      if (cur->shdr->sh_link > todel->index)
	cur->shdr->sh_link--;
      else if (cur->shdr->sh_link == todel->index)
	cur->shdr->sh_link = 0;
      if (cur->shdr->sh_offset > obj->hdr->e_shoff)
	cur->shdr->sh_offset -= obj->hdr->e_shentsize;
    }

  /* Shift SHT file offset and .shstrtab index */
  if (obj->hdr->e_shoff > shdr.sh_offset)
    obj->hdr->e_shoff -= shdr.sh_size;
  if (obj->hdr->e_shstrndx > todel->index)
    obj->hdr->e_shstrndx--;

  /* Modify segments if removed section was mapped */
  todel->phdr = elfsh_get_parent_segment(obj, todel);
  if (todel->phdr)
    {
      
      /* Reset PT_PHDR segment is section was pre-interp injected */
      for (idx = 0, pht = obj->pht; idx < obj->hdr->e_phnum; idx++)
	if (elfsh_segment_is_executable(todel->phdr) &&
	    (pht[idx].p_type == PT_PHDR || todel->phdr == pht + idx))
	  {
	    pht[idx].p_vaddr += shdr.sh_size;
	    pht[idx].p_paddr += shdr.sh_size;
	  }
	else if (todel->phdr != pht + idx && pht[idx].p_offset >= shdr.sh_offset)
	  pht[idx].p_offset -= shdr.sh_size;
      
      /* Truncate the removed section size to the segment size */
      todel->phdr->p_filesz -= shdr.sh_size;
      todel->phdr->p_memsz -= shdr.sh_size;
  
      /* Delete symbols pointing on the removed section */
      cur = obj->secthash[ELFSH_SECTION_SYMTAB];
      sym = cur->data;
      idx = 0;
      while (idx < cur->shdr->sh_size / sizeof(elfsh_Sym))
	if (INTERVAL(shdr.sh_addr, sym[idx].st_value, shdr.sh_addr + shdr.sh_size))
	  elfsh_remove_symbol(cur, elfsh_get_symbol_name(obj, sym + idx));
	else
	  idx++;
    }

  /* Free deleted section */
  XFREE(__FILE__, __FUNCTION__, __LINE__,todel->name);
  XFREE(__FILE__, __FUNCTION__, __LINE__,todel->data);
  if (todel->altdata)
    XFREE(__FILE__, __FUNCTION__, __LINE__,todel->altdata);
  if (todel->terdata)
    XFREE(__FILE__, __FUNCTION__, __LINE__,todel->terdata);
  if (todel->rel)
    XFREE(__FILE__, __FUNCTION__, __LINE__,todel->rel);
  XFREE(__FILE__, __FUNCTION__, __LINE__,todel);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Return the last section of the list 
 * @param file
 * @return
 */
elfshsect_t		*elfsh_get_tail_section(elfshobj_t *file)
{
  elfshsect_t		*s;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || file->hdr == NULL || file->sectlist == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  s = elfsh_get_section_by_index(file, file->hdr->e_shnum - 1, NULL, NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
}

/**
 * Return the last section of the runtime list 
 * @param file
 * @return
 */
elfshsect_t		*elfsh_get_tail_rsection(elfshobj_t *file)
{
  elfshsect_t		*s;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || file->hdr == NULL || file->rsectlist == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  s = elfsh_get_rsection_by_index(file, file->rhdr.rshtnbr - 1, NULL, NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
}


/*
*******************************************************************************
** ELFsh section objet handlers (see elfsh.h: elfshpath_t, elfshL1_t, elfshL2_t)
**
** These calls are probably only used by elfsh, it should go in vm/ maybe ..
*******************************************************************************
*/

/**
 * @brief Return the section list 
 * @param file
 * @param num
 * @return
 */
elfshsect_t		*elfsh_get_section_list(elfshobj_t *file, int *num)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  if (NULL == elfsh_get_sht(file, num))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get SHT", NULL);
  if (file->hdr == NULL && NULL == elfsh_get_hdr(file))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get ELF header", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->sectlist));
}

/**
 * @brief Return the section list 
 * @param file
 * @param num
 * @return
 */
elfshsect_t		*elfsh_get_rsection_list(elfshobj_t *file, int *num)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  if (NULL == elfsh_get_sht(file, num))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get SHT", NULL);
  if (file->hdr == NULL && NULL == elfsh_get_hdr(file))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get ELF header", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->rsectlist));
}


/**
 * @brief Return the section giving its index 
 * @param list
 * @param index
 * @return
 */
elfshsect_t		*elfsh_get_section_by_idx(elfshsect_t *list, 
						  eresi_Addr index)
{
  unsigned int			cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (cur = 0; cur != index && list != NULL; cur++)
    list = list->next;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (list));
}

/**
 * @brief Return the section giving its name (internal handler).
 * @param file The parent file where to read the section.
 * @param name The name of the section.
 * @return The section descriptor or NULL if error.
 */
elfshsect_t		*elfsh_get_section_by_nam(elfshobj_t *file, char *name)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_get_section_by_name(file, name, NULL, NULL, NULL)));
}

/**
 * @brief Raw read access to a section data.
 * @param obj The section to read.
 * @param off The object index to read in the section, or 1 if reading raw.
 * @param sizelem The object size within this section, or 1 if reading raw.
 * @return A pointer on the section's data or NULL if error.
 */
void		*elfsh_get_section_data(elfshsect_t *obj, unsigned int off, 
					unsigned int sizelem)
{
  char		*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (obj == NULL || obj->shdr == NULL || obj->data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  if (sizelem == 0)
    sizelem = 1;

  if (obj->shdr->sh_size <= off * sizelem)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Offset too big", NULL);

  /* Return the data */
  data = elfsh_get_raw(obj);
  data += (off * sizelem);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (data));
}

/**
 * @brief Raw acess to section data.
 * @param sect The section to access.
 * @param off The offset within the section where to write.
 * @param data The data to write (raw format).
 * @param size The number of elements to write.
 * @param sizelem The size of elements to write.
 * @return Error (-1) or Success (0).
 */
int		elfsh_write_section_data(elfshsect_t		*sect,
					 unsigned int			off,
					 char			*data,
					 unsigned int			size,
					 unsigned int			sizelem)
{
  void	*rdata;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sect == NULL || sect->shdr == NULL || sect->data == NULL || data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  if (sizelem == 0)
    sizelem = 1;
  if ((off * sizelem) + size > sect->shdr->sh_size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section is too small", -1);

  rdata = elfsh_get_raw(sect);
  memcpy(rdata + (off * sizelem), data, size);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Tell if a section exists on disk.
 * @param sect The section
 * @return Exist (0) or does not exist (1) on disk.
 */
int		elfsh_section_is_runtime(elfshsect_t *sect)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (sect && (sect->flags & ELFSH_SECTION_RUNTIME))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
