/*
** section.c for libelfsh
**
** Started on  Mon Feb 26 04:12:42 2001 mayhem
** Last update Thu Aug 21 02:56:04 2003 jv
*/
#include "libelfsh.h"




/* Just used to avoid code redundancy in elfsh_add_section() */
char	elfsh_shift_section(elfshsect_t *sct, elfshsect_t *tmp, u_char mode)
{
  /* Used by SHT reconstruction engine */
  if (mode >= ELFSH_SHIFTING_NONE &&
      tmp->shdr->sh_link && (sct->index <= tmp->shdr->sh_link))
    tmp->shdr->sh_link++;

  /* Unmapped section shifting */
  if (mode >= ELFSH_SHIFTING_PARTIAL)
    {
      tmp->shdr->sh_offset += sct->shdr->sh_size;
      tmp->index++;
    }

  /* Mapped section shifting */
  if (mode >= ELFSH_SHIFTING_COMPLETE && tmp->shdr->sh_addr)
      tmp->shdr->sh_addr += sct->shdr->sh_size;

  /* Return 1 */
  return (1);
}






/* Return a pointer on a section giving its name */
elfshsect_t	*elfsh_get_section_by_name(elfshobj_t	*file,
					   char		*name,
					   int		*idx,
					   int		*strindex,
					   int		*num)
{
  elfshsect_t	*section;
  char		*sname;
  int		index;

  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_section_by_name] Invalid NULL parameter\n",
		   NULL);

  else if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    ELFSH_SETERROR("[libelfsh:get_section_by_name] No SHT\n", NULL);

  for (index = 0, section = file->sectlist;
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
	    *num = section->shdr->sh_size;
	  if (strindex != NULL)
	    *strindex = section->shdr->sh_link;
	  return (section);
	}
    }

  ELFSH_SETERROR("[libelfsh:get_section_by_name] Section not found\n", NULL);
}






/* Return the section header for the 'range' occurence of a 'type typed section */
/* Return NULL if failed */
elfshsect_t		*elfsh_get_section_by_type(elfshobj_t	*file,
						   u_int	type,
						   int		range,
						   int		*index,
						   int		*strindex,
						   int		*num)

{
  int			number;
  int			local_index;
  int			nbr;
  elfshsect_t		*section;

  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_section_by_type] Invalid NULL parameter\n",
		   NULL);
  else if (elfsh_get_sht(file, &nbr) == NULL)
    return (NULL);

  /* Parse the section list */
  for (section = file->sectlist, local_index = number = 0;
       section != NULL && local_index < nbr;
       local_index++, section = section->next)
    {

      if (local_index == file->hdr->e_shnum)
	ELFSH_SETERROR("[libelfsh:get_section_by_type] Section not found\n",
		       NULL);

      if (section->shdr->sh_type == type && ++number > range)
	{
	  if (strindex != NULL)
	    *strindex = section->shdr->sh_link;
	  if (num != NULL)
	    *num = section->shdr->sh_size;
	  if (index != NULL)
	    *index = local_index;
	  return (section);
	}

    }
  return (NULL);
}






/* Return a Elf32_Shdr pointer giving the section index in the sht */
elfshsect_t		*elfsh_get_section_by_index(elfshobj_t	*file,
						    int		index,
						    int		*strindex,
						    int		*num)
{
  elfshsect_t		*section;
  int			local;

  if (NULL == file)
    ELFSH_SETERROR("[libelfsh:get_section_by_index] Invalid NULL parameter",
		   NULL);
  else if (NULL == elfsh_get_sht(file, num))
    return (NULL);

  for (section = file->sectlist, local = 0;
       NULL != section && local < index; local++)
    section = section->next;

  if (section == NULL)
    ELFSH_SETERROR("[libelfsh:get_section_by_index] Cannot find section\n",
		   NULL);

  if (strindex != NULL)
    *strindex = file->sht[index].sh_link;
  if (num != NULL)
    *num = file->sht[index].sh_size;

  return (section);
}







/*
** Load the code of a section giving its header
** Should be called after a elfsh_get_section_by_{name,type} to load the code
*/
void		*elfsh_load_section(elfshobj_t *file, Elf32_Shdr *shdr)
{
  void		*sct;

  if (shdr == NULL || !shdr->sh_size)
    ELFSH_SETERROR("[libelfsh:load_section] Cannot load NULL section\n", NULL);
  XSEEK(file->fd, shdr->sh_offset, SEEK_SET, NULL);
  XALLOC(sct, shdr->sh_size, NULL);
  XREAD(file->fd, sct, shdr->sh_size, NULL);
  return (sct);
}






/* Fill an anonymous (unknown content) section */
void		*elfsh_get_anonymous_section(elfshobj_t *file, elfshsect_t *sect)
{

  /* Bad parameter checking */
  if (file == NULL || sect == NULL)
    ELFSH_SETERROR("[libelfsh:get_anonymous_section] invalid NULL parameter",
		   NULL);

  /* If the section is already loaded */
  if (sect->data != NULL)
    return (sect->data);

  sect->data = elfsh_load_section(file, sect->shdr);
  return (sect->data);
}





/*
**
** INTERNAL FUNCTION : Add a given section to the file's section list
**
** SHOULD -NOT- manipulation symbol table because we are called from
** elfsh_load_sht() which has not loaded symtab yet.
**
*/
int		elfsh_add_section(elfshobj_t	*file,
				  elfshsect_t	*sct,
				  u_int		range,
				  void		*dat,
				  int		shiftmode)
{
  elfshsect_t	*tmp;
  char		inserted;

  /* Load the ELF header if not done */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:add_section] Invalid NULL parameter\n", -1);
  else if (file->sht == NULL)
    ELFSH_SETERROR("[libelfsh:add_section] SHT not loaded\n", -1);

  /* Refuse to insert a section for some conditions */
  if (range >= file->hdr->e_shnum)
    ELFSH_SETERROR("[libelfsh:add_section] Unknown SHT slot\n", -1);
  else if (sct->flags & ELFSH_SECTION_INSERTED)
      ELFSH_SETERROR("[libelfsh:add_section] Already inserted\n", -1);
  else if (sct->data != NULL)
    ELFSH_SETERROR("[libelfsh:add_section] Section is not empty\n", -1);

  sct->shdr   = file->sht + range;
  sct->parent = file;
  sct->index  = range;
  sct->data   = dat;
  inserted    = 0;

#if __DEBUG_SECTS__
  printf("[DEBUG:add_section] Adding section [index = %u] in file %s \n",
	 sct->index, file->name);
#endif

  /* Parse the section list */
  for (tmp = file->sectlist; tmp != NULL && tmp->next != NULL; tmp = tmp->next)
    if (tmp->index == range)
      {
	/* Insert the new section */
	sct->prev = tmp->prev;
	sct->next = tmp;
	if (sct->prev != NULL)
	  sct->prev->next = sct;
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
      file->sectlist = sct;
      file->sectlist->prev = sct;
    }

  /* The section must be inserted at the last place */
  else if (!inserted && !tmp->next)
    {
      tmp->next = sct;
      sct->prev = tmp;
      file->sectlist->prev = sct;
    }

  /* shift the last section if the insertion has already been done */
  else if (inserted && !tmp->next)
    elfsh_shift_section(sct, tmp, shiftmode);

  /* Final fixes */
  sct->phdr   = elfsh_get_parent_segment(file, sct);
  sct->flags |= ELFSH_SECTION_INSERTED;
  return (0);
}







/* Creation a new orphelin section */
elfshsect_t	*elfsh_create_section(char *name)
{
  elfshsect_t	*new;

  if (name == NULL)
    ELFSH_SETERROR("[libelfsh:create_section] Invalid NULL name\n", NULL);
  XALLOC(new, sizeof(elfshsect_t), NULL);
  new->name = strdup(name);
  return (new);
}




/* Return the parent section for this virtual address */
elfshsect_t	*elfsh_get_parent_section(elfshobj_t	*file,
					  u_int		value,
					  int		*offset)
{
  elfshsect_t	*s;

  if (file == NULL || NULL == value ||
      (NULL == file->sht && !elfsh_get_sht(file, NULL)))
    return (NULL);

  for (s = file->sectlist; s; s = s->next)
    if (INTERVAL(s->shdr->sh_addr, value,
		 (s->shdr->sh_addr + s->shdr->sh_size)))
      {
	if (offset)
	  *offset = value - s->shdr->sh_addr;
	return (s);
      }
  return (NULL);
}





/* Return the parent section for this file offset, else NULL */
elfshsect_t	*elfsh_get_parent_section_by_foffset(elfshobj_t *file,
						     u_int	foff,
						     int	*offset)
{
  elfshsect_t	*s;
  char		i;
  u_int		bval;

  if (file == NULL || elfsh_get_sht(file, NULL) == NULL)
    return (NULL);

  for (s = file->sectlist; s; s = s->next)
    {
      bval = (s->next != NULL ? s->next->shdr->sh_offset :
	      s->shdr->sh_offset + s->shdr->sh_size);
      i = INTERVAL(s->shdr->sh_offset, foff, bval);
      if (i && elfsh_get_section_type(s->shdr) != SHT_NOBITS)
	{
	  if (offset != NULL)
	    *offset = foff - s->shdr->sh_offset;
	  return (s);
	}
    }
  return (NULL);
}




/* Retreive the section giving the section symbol from .symtab */
elfshsect_t		*elfsh_get_section_from_sym(elfshobj_t *file, Elf32_Sym *sym)
{
  elfshsect_t		*current;

  if (file == NULL || sym == NULL)
    ELFSH_SETERROR("[libelfsh:get_section_from_sym] Invalid NULL parameter\n",
		   NULL);
  if (NULL == elfsh_get_sht(file, NULL))
    return (NULL);
  for (current = file->sectlist; current != NULL; current = current->next)
    if (current->shdr->sh_addr == sym->st_value)
      return (current);
  ELFSH_SETERROR("[libelfsh:get_section_from_sym] No correspondance\n", NULL);
}






/* Append data to a section */
int		elfsh_append_data_to_section(elfshsect_t	*sect,
					     void		*input,
					     u_int		len)
{
  elfshsect_t	*actual;

  /* Sanity checks */
  if (sect == NULL || input == NULL || !len)
    ELFSH_SETERROR("[libelfsh:append_data_to_section] Invalid NULL parameter",
		   -1);

  /* Put the data at the end of the section */
  if (sect->data != NULL)
    XREALLOC(sect->data, sect->data, sect->shdr->sh_size + len, -1);
  else
    XALLOC(sect->data, len, -1);
  memcpy(sect->data + sect->shdr->sh_size, input, len);
  sect->shdr->sh_size += len;


  if (sect->parent->rights == O_RDONLY)
    {
      printf("[DEBUG_APPEND] NOT SHIFTING FILE BECAUSE RO ACCESS on %s:%s\n",
	     sect->parent->name, sect->name);
      goto end;
    }

  /* Modify the section header table file offset if necessary */
  if (sect->parent->hdr->e_shoff >= sect->shdr->sh_offset)
    sect->parent->hdr->e_shoff += len;

  /* Update vaddr and foffset for all next sections */
  for (actual = sect; actual != NULL; actual = actual->next)
    if (actual != sect)
      {
	if (actual->shdr->sh_addr != NULL)
	  actual->shdr->sh_addr += len;
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
  return (sect->shdr->sh_size - len);
}



/* Insert a new section at the first place in the executable PT_LOAD */
int		elfsh_insert_code_section(elfshobj_t	*file,
					  elfshsect_t	*sect,
					  Elf32_Shdr	hdr,
					  void		*data)
{
  elfshsect_t	*first;
  Elf32_Phdr	*phdr;
  Elf32_Phdr	*cur;
  int		range;
  char		*rdata;
  u_int		rsize;

  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    return (-1);

  /* Find the first executable PT_LOAD segment */
  range = 0;
  do
    {
      phdr = elfsh_get_segment_by_type(file, PT_LOAD, range);
      range++;
    }
  while (phdr && !elfsh_segment_is_executable(phdr));
  if (phdr == NULL)
    ELFSH_SETERROR("[libelfsh:insert_code_section] Cannot find +x PT_LOAD", -1);

  /* Find the first allocatable section */
  first = file->sectlist;
  while (!first->shdr->sh_addr)
    first = first->next;
  if (first == NULL)
    ELFSH_SETERROR("[libelfsh:insert_code_section] Cannot find first section\n",
		   -1);

  /* Pad the new section if needed */
  /* XXX: rdata need to be freed */
  if (hdr.sh_size % ELFSH_PAGESIZE)
    {
      rsize = hdr.sh_size + ELFSH_PAGESIZE - (hdr.sh_size % ELFSH_PAGESIZE);
      XALLOC(rdata, rsize, -1);
      memcpy(rdata, data, hdr.sh_size);
      hdr.sh_size = rsize;
      data = rdata;
    }

  /* Extend the first loadable segment from the top */
  hdr.sh_addr = first->shdr->sh_addr - hdr.sh_size;
  hdr.sh_offset = first->shdr->sh_offset;
  phdr->p_filesz += hdr.sh_size;
  phdr->p_memsz  += hdr.sh_size;
  phdr->p_vaddr  -= hdr.sh_size;
  phdr->p_paddr  -= hdr.sh_size;

  /* Fixup file offset for all loadable segments and fixup PHDR base vaddr */
  for (range = 0, cur = file->pht; range < file->hdr->e_phnum; range++)
    if (cur[range].p_type == PT_PHDR)
      {
	cur[range].p_vaddr -= hdr.sh_size;
	cur[range].p_paddr -= hdr.sh_size;
      }
    else if (cur != phdr && cur[range].p_offset >= hdr.sh_offset)
      cur[range].p_offset += hdr.sh_size;

  /* Inject our section with the associated header */
  if (elfsh_insert_section_header(file, hdr, first->index, sect->name) < 0)
    return (-1);
  if (elfsh_add_section(file, sect, first->index,
			data, ELFSH_SHIFTING_PARTIAL) < 0)
    return (-1);

  /* Inject the symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    return (-1);

  return (sect->index);
}


/* Insert a data section in the object */
int		elfsh_insert_data_section(elfshobj_t	*file,
					  elfshsect_t	*sect,
					  Elf32_Shdr	hdr,
					  void		*data)
{
  elfshsect_t	*last;
  void		*rdata;

  /* Sanity checks */
  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    return (-1);

  /* Insert the bss physically in the file if not already done */
  if (elfsh_fixup_bss(file) < 0)
    ELFSH_SETERROR("[libelfsh:insert_mapped_section] Cannot fixup .bss\n", -1);

  /* Iterate and get the real last mapped section */
  last = file->sectlist;
  while (last->next != NULL && last->next->shdr->sh_addr != NULL)
    last = last->next;

  /* Extend the segment to insert the new section */
  hdr.sh_addr = last->shdr->sh_addr + last->shdr->sh_size;
  hdr.sh_offset = last->shdr->sh_offset + last->shdr->sh_size;
  last->phdr->p_filesz += hdr.sh_size;
  last->phdr->p_memsz  += hdr.sh_size;

  /* Copy the data */
  XALLOC(rdata, hdr.sh_size, -1);
  memcpy(rdata, data, hdr.sh_size);

  /* Inject our section with the associated header */
  if (elfsh_insert_section_header(file, hdr, last->index + 1, sect->name) < 0)
    return (-1);
  if (elfsh_add_section(file, sect, last->index + 1,
			rdata, ELFSH_SHIFTING_COMPLETE) < 0)
    return (-1);

  /* Inject the symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    return (-1);

  return (sect->index);
}



/* Insert a mapped section in the object */
int		elfsh_insert_mapped_section(elfshobj_t	*file,
					    elfshsect_t *sect,
					    Elf32_Shdr	hdr,
					    void	*data,
					    int		mode)
{

  /* Preliminary checks */
  if (file == NULL || sect == NULL)
    ELFSH_SETERROR("[libelfsh:insert_mapped_section] Invalid NULL parameter\n",
		   -1);
  switch (mode)
    {
    case ELFSH_CODE_INJECTION:
      return (elfsh_insert_code_section(file, sect, hdr, data));
    case ELFSH_DATA_INJECTION:
      return (elfsh_insert_data_section(file, sect, hdr, data));
    default:
      ELFSH_SETERROR("[libelfsh:insert_mapped_section] Unknown mode\n", -1);
    }
}


/* Insert a non-mapped section in the object */
int		elfsh_insert_unmapped_section(elfshobj_t	*file,
					      elfshsect_t	*sect,
					      Elf32_Shdr	hdr,
					      void		*data)
{
  elfshsect_t	*s;

  /* Preliminary checks */
  if (file == NULL || sect == NULL)
    ELFSH_SETERROR("[libelfsh:insert_unmapped_section] Invalid NULL param.\n",
		   -1);
  /* Sanity checks */
  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    return (-1);

  /* Get the last current section */
  s = elfsh_get_section_by_index(file, file->hdr->e_shnum - 1, NULL, NULL);
  if (s == NULL)
    return (-1);

  /* Fix the file offset for the new section */
  hdr.sh_offset = s->shdr->sh_offset + s->shdr->sh_size;

  /* Make sure the new unmapped section do not overlap SHT */
  if (hdr.sh_offset <= file->hdr->e_shoff &&
      hdr.sh_offset + hdr.sh_size >= file->hdr->e_shoff)
    hdr.sh_offset = file->hdr->e_shoff + (file->hdr->e_shnum *
					  file->hdr->e_shentsize);


  /* Inject the new section and the associated header */
  if (elfsh_insert_section_header(file, hdr, file->hdr->e_shnum,
				  sect->name) < 0)
    return (-1);

  if (elfsh_add_section(file, sect, file->hdr->e_shnum - 1,
			data, ELFSH_SHIFTING_COMPLETE) < 0)
    return (-1);

  /* Inject the symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    return (-1);

  return (sect->index);
}



/* Insert a section at the requested index */
int		elfsh_insert_section(elfshobj_t		*file,
				     elfshsect_t	*sect,
				     Elf32_Shdr		hdr,
				     void		*data,
				     u_int		index)
{
  elfshsect_t	*s;

  /* Preliminary checks */
  if (file == NULL || sect == NULL)
    ELFSH_SETERROR("[libelfsh:insert_unmapped_section] Invalid NULL param.\n",
		   -1);
  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    return (-1);

  /* Get the last current section */
  s = elfsh_get_section_by_index(file, index - 1, NULL, NULL);
  if (s == NULL)
    return (-1);

  /* Fix the file offset for the new section */
  hdr.sh_offset = s->shdr->sh_offset + s->shdr->sh_size;
  if (s->shdr->sh_addr)
    hdr.sh_addr = s->shdr->sh_addr + s->shdr->sh_size;

  /* Inject the new section and the associated header */
  if (elfsh_insert_section_header(file, hdr, index, sect->name) < 0)
    return (-1);

  /* Add the section with absolute shifting */
  if (elfsh_add_section(file, sect, index, data, ELFSH_SHIFTING_COMPLETE) < 0)
    return (-1);

  /* Inject the symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    return (-1);

  return (sect->index);
}





/* Return the last section of the list */
elfshsect_t		*elfsh_get_tail_section(elfshobj_t *file)
{
  elfshsect_t		*s;

  if (file == NULL || file->hdr == NULL || file->sectlist == NULL)
    ELFSH_SETERROR("[libelfsh:get_tail_section] Invalid NULL parameter\n",
		   NULL);
  s = elfsh_get_section_by_index(file, file->hdr->e_shnum - 1, NULL, NULL);
  return (s);
}



/*
**
**
** ELFsh section objet handlers (see elfsh.h: elfshpath_t, elfshL1_t, elfshL2_t)
**
** These calls should not be used by something else than elfsh .
**
**
*/



/* Return the section list */
elfshsect_t		*elfsh_get_section_list(elfshobj_t *file, int *num)
{
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_section_list] Invalid NULL parameter\n",
		   NULL);
  if (NULL == elfsh_get_sht(file, num))
    return (NULL);
  if (file->hdr == NULL && NULL == elfsh_get_hdr(file))
    return (NULL);
  return (file->sectlist);
}


/* Return the section giving its index */
elfshsect_t		*elfsh_get_section_by_idx(elfshsect_t *list, int index)
{
  u_int			cur;

  for (cur = 0; cur != index && list != NULL; cur++)
    list = list->next;
  return (list);
}

/* Return the section giving its name */
elfshsect_t		*elfsh_get_section_by_nam(elfshobj_t *file, char *name)
{
  return (elfsh_get_section_by_name(file, name, NULL, NULL, NULL));
}

/* Section objects 'raw' child read access */
void		*elfsh_get_section_data(elfshsect_t *obj, u_int off, u_int sizelem)
{
  char		*data;

  /* Sanity checks */
  if (obj == NULL || obj->shdr == NULL || obj->data == NULL)
    ELFSH_SETERROR("[libelfsh:get_section_data] Invalid NULL parameter\n",
		   NULL);

  if (sizelem == 0)
    sizelem = 1;

  if (obj->shdr->sh_size <= off * sizelem)
    ELFSH_SETERROR("[libelfsh:get_section_data] Offset too big\n", NULL);

  /* Return the data */
  data = obj->data;
  data += (off * sizelem);
  return (data);
}



/* Section objects 'raw' child write access */
int		elfsh_write_section_data(elfshsect_t		*sect,
					 u_int			off,
					 char			*data,
					 u_int			size,
					 u_int			sizelem)
{

  if (sect == NULL || sect->shdr == NULL || sect->data == NULL || data == NULL)
    ELFSH_SETERROR("[libelfsh:write_section_data] Invalid NULL paramater\n",
		   -1);
  if (sizelem == 0)
    sizelem = 1;
  if ((off * sizelem) + size > sect->shdr->sh_size)
    ELFSH_SETERROR("[libelfsh:write_section_data] Section is too small\n", -1);
  memcpy(sect->data + (off * sizelem), data, size);
  return (0);
}
