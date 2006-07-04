/*
** section.c for libelfsh
**
** All handlers and lowlevel routines for sections management
**
** Started on  Mon Feb 26 04:12:42 2001 mayhem
**
*/
#include "libelfsh.h"




/* Just used to avoid code redundancy in elfsh_add_section() */
char	elfsh_shift_section(elfshsect_t *sct, elfshsect_t *tmp, u_char mode)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
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
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}





/*
**
** INTERNAL FUNCTION : Add a given section to the file's section list
**
** SHOULD -NOT- manipulation symbol table because we are called from
** elfsh_load_sht() which has not loaded symtab yet.
**
** This function is used for constructing the map (elfshobj_t) of a 
** binary file in memory. It is called as well at each new section
** injection.
**
** See libelfsh/inject.c for the user-friendly section injection API
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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  
#if __DEBUG_RUNTIME__
  printf("[DEBUG_RUNTIME] Adding section with range = %u, file->rhdr.rshtnbr = %u, shnum = %u (%s) \n", 
	 range, file->rhdr.rshtnbr, file->hdr->e_shnum, sct->name);
#endif

  /* Load the ELF header if not done */
  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (file->sht == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "SHT not loaded", -1);

  else if (elfsh_section_is_runtime(sct) && !file->rsht)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "RSHT not loaded", -1);

  /* Refuse to insert a section for some conditions */
  if ((elfsh_section_is_runtime(sct) && range >= file->rhdr.rshtnbr) ||
      range >= file->hdr->e_shnum)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown requested section slot", -1);

  else if (sct->flags & ELFSH_SECTION_INSERTED)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Already inserted", -1);
  else if (sct->data != NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
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

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Add a runtime section */
/* This function is internal, do not use it directly */
int             elfsh_add_runtime_section(elfshobj_t    *file,
					  elfshsect_t   *sct,
					  u_int         range,
					  void          *dat)
{
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We do not shift any file offset in runtime injection */
  sct->flags |= ELFSH_SECTION_RUNTIME;
  ret = elfsh_add_section(file, sct, range, dat, ELFSH_SHIFTING_NONE);
  if (!ret)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
  else
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot add runtime section", ret);
}






/* Internal function */
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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (section));
	}
    }
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Section not found", NULL);
}



/* Return a pointer on a section giving its name */
elfshsect_t	*elfsh_get_section_by_name(elfshobj_t	*file,
					   char		*name,
					   int		*idx,
					   int		*strindex,
					   int		*num)
{
  elfshsect_t	*sect;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);

  else if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No SHT", NULL);

  sect = elfsh_get_section_by_name_withlist(file, name, idx, strindex, num, 
					    file->sectlist);
  if (!sect && file->rsectlist)
    sect = elfsh_get_section_by_name_withlist(file, name, idx, strindex, 
					      num, file->rsectlist);

  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section not found", NULL);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect));
}



/* Search section by type, with list parameter */
/* This is an internal function it should not be used directly */
static
elfshsect_t		*elfsh_get_section_by_type_withlist(elfshobj_t	*file,
							    u_int	type,
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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Parse the section list */
  for (section = sectlist, local_index = number = 0;
       section != NULL && local_index < shnum;
       local_index++, section = section->next)
    {
      
      if (local_index == shnum)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
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
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (section));
	}

    }

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Section not found ..", NULL);
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
  elfshsect_t		*section;
  int			nbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
		      
  else if (elfsh_get_sht(file, &nbr) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
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
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section remained unfound", NULL);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (section));  
}






/* Return the section pointer giving the section index in the sht */
elfshsect_t		*elfsh_get_section_by_index(elfshobj_t	*file,
						    elfsh_Addr	index,
						    int		*strindex,
						    int		*num)
{
  elfshsect_t		*section;
  int			local;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == file)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  else if (NULL == elfsh_get_sht(file, num))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive SHT", NULL);

  for (section = file->sectlist, local = 0;
       NULL != section && local < index; local++)
    section = section->next;

  if (section == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find section", NULL);

  if (strindex != NULL)
    *strindex = file->sht[index].sh_link;
  if (num != NULL)
    *num = (section->curend ?
	    section->curend : section->shdr->sh_size);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (section));
}



/* Same that index research but for runtime injected sections */
elfshsect_t		*elfsh_get_rsection_by_index(elfshobj_t	*file,
						    elfsh_Addr	index,
						    int		*strindex,
						    int		*num)
{
  elfshsect_t		*section;
  int			local;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == file)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  else if (NULL == elfsh_get_runtime_sht(file, num))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive RSHT", NULL);

  for (section = file->rsectlist, local = 0;
       NULL != section && local < index; local++)
    section = section->next;

  if (section == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find runtime section", NULL);

  if (strindex != NULL)
    *strindex = file->rsht[index].sh_link;
  if (num != NULL)
    *num = (section->curend ?
	    section->curend : section->shdr->sh_size);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (section));
}







/*
** Load the code of a section giving its header
** Should be called after a elfsh_get_section_by_{name,type} to load the code
*/
void		*elfsh_load_section(elfshobj_t *file, elfsh_Shdr *shdr)
{
  void		*sct;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (shdr == NULL || !shdr->sh_size)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot load NULL section", NULL);

  XSEEK(file->fd, shdr->sh_offset, SEEK_SET, NULL);
  XALLOC(sct, shdr->sh_size, NULL);
  XREAD(file->fd, sct, shdr->sh_size, NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (sct));
}






/* Fill an anonymous (unknown content) section */
void		*elfsh_get_anonymous_section(elfshobj_t *file, elfshsect_t *sect)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Bad parameter checking */
  if (file == NULL || sect == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* If the section is already loaded */
  if (sect->data != NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_raw(sect)));

  sect->data = elfsh_load_section(file, sect->shdr);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_raw(sect)));
}







/* Creation a new orphelin section */
elfshsect_t	*elfsh_create_section(char *name)
{
  elfshsect_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL name", NULL);
  XALLOC(new, sizeof(elfshsect_t), NULL);
  new->name = elfsh_strdup(name);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}



/* Return the parent section for this virtual address */
elfshsect_t	*elfsh_get_parent_section(elfshobj_t	*file,
					  elfsh_Addr   	value,
					  elfsh_SAddr   *offset)
{
  elfshsect_t	*s;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (file == NULL || NULL == value)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);    

  if (NULL == file->sht && !elfsh_get_sht(file, NULL))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
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
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
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
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
      }

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to get parent section", NULL);
}





/* Return the parent section for this file offset, else NULL */
/* This function is only revelant for ondisk sections */
elfshsect_t	*elfsh_get_parent_section_by_foffset(elfshobj_t *file,
						     u_int	foff,
						     elfsh_SAddr *offset)
{
  elfshsect_t	*s;
  char		i;
  u_int		bval;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || elfsh_get_sht(file, NULL) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
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
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
	}
    }
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Parent section remained unfound", NULL);
}




/* Retreive the section giving the section symbol from .symtab */
elfshsect_t		*elfsh_get_section_from_sym(elfshobj_t *file, 
						    elfsh_Sym *sym)
{
  elfshsect_t		*current;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || sym == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  if (NULL == elfsh_get_sht(file, NULL))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Cannot get SHT", NULL);

  /* Compare ondisk sections first */
  for (current = file->sectlist; current != NULL; current = current->next)
    if (current->shdr->sh_addr == sym->st_value)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (current));

  /* Compare runtime injected section is still unfound */
  for (current = file->rsectlist; current != NULL; current = current->next)
    if (current->shdr->sh_addr == sym->st_value)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (current));

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "No correspondance", NULL);
}




/* This function appends a single char to a section by extending it */
int		elfsh_fill_section(elfshsect_t	*sect, 
				   char		c,
				   u_int	size)
{
  char		*str;
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  str = alloca(size);
  memset(str, c, size);
  ret = elfsh_append_data_to_section(sect, str, size);
  if (ret <= 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to append data to section", NULL);
  else
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}




/* 
   This function is quite context dependant and should be rewritten.
   Symbol table shifting might be avoided in some obscure
   not-yet-encountered cases. For now it works as requested quite
   correctly.

   Note : Section placed before non-relocatable sections should
   not use this function because all mapped following sections
   virtual addresses will change. Be sure to have decent 
   (recovered) relocation tables before using it on those sections.

   Only strong dataflow engine seems to be the solution for those.

   This function is not e2dbg safe and should only be used ondisk !
*/
int		elfsh_append_data_to_section(elfshsect_t	*sect,
					     void		*input,
					     u_int		len)
{
  elfshsect_t	*actual;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Sanity checks */
  if (sect == NULL || input == NULL || !len)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  /*
  printf("Extending %s section from %u bytes (curend = %u) \n", 
	 sect->name, len, sect->curend);
  fflush(stdout);
  */

  /* Put the data at the end of the section */
  /* XXX: need to re-mmap in the case of runtime section */
  if (sect->curend + len > sect->shdr->sh_size)
    {
      if (elfsh_section_is_runtime(sect) && sect->shdr->sh_addr)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot extend a runtime injected zone", -1);

      if (sect->data != NULL)
	XREALLOC(sect->data, sect->data, sect->shdr->sh_size + len, -1);
      else
	XALLOC(sect->data, len, -1);
      
      memcpy(sect->data + sect->shdr->sh_size, input, len);
      sect->shdr->sh_size += len;
      sect->curend += len;
    }
  
  /* Enough room in the section to put our data */
  else
    {
      memcpy(sect->data + sect->curend, input, len);
      sect->curend += len;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->curend - len));
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
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (sect->shdr->sh_size - len));
}




/* Remove a section */
/* XXX-runtime : This section should only be used for ondisk modifications */
int			elfsh_remove_section(elfshobj_t *obj, char *name)
{
  elfshsect_t		*todel;
  elfshsect_t		*cur;
  elfsh_Shdr		shdr;
  elfsh_Shdr		*new;
  elfsh_Phdr		*pht;
  elfsh_Sym		*sym;
  u_int			idx;
  u_int			size;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (obj == NULL || name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);

  todel = elfsh_get_section_by_name(obj, name, NULL, NULL, NULL);
  if (todel == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown section to remove", -1);

  if ((todel->prev && todel->prev->shdr->sh_addr) &&
      (todel->next && todel->next->shdr->sh_addr))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
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
  XALLOC(new, obj->hdr->e_shnum * sizeof(elfsh_Shdr), -1);
  memcpy(new, obj->sht, obj->hdr->e_shnum * sizeof(elfsh_Shdr));
  XFREE(obj->sht);
  obj->sht = new;
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
  XFREE(todel->name);
  XFREE(todel->data);
  if (todel->altdata)
    XFREE(todel->altdata);
  if (todel->terdata)
    XFREE(todel->terdata);
  if (todel->rel)
    XFREE(todel->rel);
  XFREE(todel);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Nice embedded debugging trick */
/* Return a pointer on the section data */
/* This function makes the difference between data and pdata, beeing the process data */
void			*elfsh_get_raw(elfshsect_t *sect)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_is_debug_mode())
    {
      //printf("entering raw debug mode (sect = %08X) \n", sect);
      //fflush(stdout);
      //printf("%08X + base %08X \n", sect->shdr->sh_addr, sect->parent->rhdr.base);

      sect->pdata = (void *) sect->shdr->sh_addr;
      if (!elfsh_section_is_runtime(sect))
	sect->pdata += sect->parent->rhdr.base;

      // For unmapped sections
      if (!sect->pdata)
	sect->pdata = sect->data;

      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->pdata));
    }
  if (sect)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->data));

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid parameter", NULL);
}




/* Return the last section of the list */
elfshsect_t		*elfsh_get_tail_section(elfshobj_t *file)
{
  elfshsect_t		*s;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || file->hdr == NULL || file->sectlist == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  s = elfsh_get_section_by_index(file, file->hdr->e_shnum - 1, NULL, NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
}

/* Return the last section of the runtime list */
elfshsect_t		*elfsh_get_tail_rsection(elfshobj_t *file)
{
  elfshsect_t		*s;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || file->hdr == NULL || file->rsectlist == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  s = elfsh_get_rsection_by_index(file, file->rhdr.rshtnbr - 1, NULL, NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
}





/*
*******************************************************************************
** ELFsh section objet handlers (see elfsh.h: elfshpath_t, elfshL1_t, elfshL2_t)
**
** These calls are probably only used by elfsh, it should go in vm/ maybe ..
*******************************************************************************
*/



/* Return the section list */
elfshsect_t		*elfsh_get_section_list(elfshobj_t *file, int *num)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  if (NULL == elfsh_get_sht(file, num))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get SHT", NULL);
  if (file->hdr == NULL && NULL == elfsh_get_hdr(file))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get ELF header", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->sectlist));
}



/* Return the section list */
elfshsect_t		*elfsh_get_rsection_list(elfshobj_t *file, int *num)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  if (NULL == elfsh_get_sht(file, num))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get SHT", NULL);
  if (file->hdr == NULL && NULL == elfsh_get_hdr(file))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get ELF header", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->rsectlist));
}


/* Return the section giving its index */
elfshsect_t		*elfsh_get_section_by_idx(elfshsect_t *list, 
						  elfsh_Addr index)
{
  u_int			cur;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (cur = 0; cur != index && list != NULL; cur++)
    list = list->next;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (list));
}

/* Return the section giving its name */
elfshsect_t		*elfsh_get_section_by_nam(elfshobj_t *file, char *name)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_get_section_by_name(file, name, NULL, NULL, NULL)));
}

/* Section objects 'raw' child read access */
void		*elfsh_get_section_data(elfshsect_t *obj, u_int off, 
					u_int sizelem)
{
  char		*data;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (obj == NULL || obj->shdr == NULL || obj->data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  if (sizelem == 0)
    sizelem = 1;

  if (obj->shdr->sh_size <= off * sizelem)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Offset too big", NULL);

  /* Return the data */
  data = elfsh_get_raw(obj);
  data += (off * sizelem);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (data));
}

/* Section objects 'raw' child write access */
int		elfsh_write_section_data(elfshsect_t		*sect,
					 u_int			off,
					 char			*data,
					 u_int			size,
					 u_int			sizelem)
{
  void	*rdata;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sect == NULL || sect->shdr == NULL || sect->data == NULL || data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL paramater", -1);

  if (sizelem == 0)
    sizelem = 1;
  if ((off * sizelem) + size > sect->shdr->sh_size)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section is too small", -1);

  rdata = elfsh_get_raw(sect);
  memcpy(rdata + (off * sizelem), data, size);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Is it a runtime section ? */
int		elfsh_section_is_runtime(elfshsect_t *sect)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (sect && (sect->flags & ELFSH_SECTION_RUNTIME))
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
