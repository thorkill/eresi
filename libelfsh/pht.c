/**
 * @file pht.c
 * @ingroup libelfsh
** pht.c for libelfsh
** 
** Started on  Mon Feb 26 04:07:33 2001 jfv
** 
**
** $Id: pht.c,v 1.7 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"



/**
 * Check if segment is readable 
 * @param p
 * @return
 */
int	elfsh_segment_is_readable(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((p->p_flags & PF_R)));
}

/**
 * Check if segment is writable
 * @param p
 * @return
 */
int	elfsh_segment_is_writable(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((p->p_flags & PF_W)));
}
/**
 * Check if segment is executable
 * @param p
 * @return
 */
int	elfsh_segment_is_executable(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((p->p_flags & PF_X)));
}

/**
 * Tiny write access functions for program headers 
 * @param p
 * @param flags
 * @return
 */
int	elfsh_set_segment_flags(elfsh_Phdr *p, eresi_Addr flags)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);

  p->p_flags = (elfsh_Word) flags;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Documentation missing.
 * @param p
 * @return
 */
elfsh_Word	elfsh_get_segment_flags(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_flags));
}

/**
 * Documentation missing.
 * @param p
 * @param align
 * @return
 */
int	elfsh_set_segment_align(elfsh_Phdr *p, eresi_Addr align)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_align = (elfsh_Word) align;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
/**
 * Documentation missing.
 * @param p
 * @return
 */
elfsh_Word	elfsh_get_segment_align(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_align));
}

/**
 * Documentation missing.
 * @param p
 * @param memsz
 * @return
 */
int	elfsh_set_segment_memsz(elfsh_Phdr *p, eresi_Addr memsz)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_memsz = (elfsh_Word) memsz;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Documentation missing.
 * @param p
 * @return
 */
elfsh_Word	elfsh_get_segment_memsz(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_memsz));
}

/**
 * Documentation missing.
 * @param p
 * @param filesz
 * @return
 */
int	elfsh_set_segment_filesz(elfsh_Phdr *p, eresi_Addr filesz)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_filesz = (elfsh_Word) filesz;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Documentation missing.
 * @param p
 * @return
 */
elfsh_Word	elfsh_get_segment_filesz(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_filesz));
}

/**
 * Documentation missing.
 * @param p
 * @param paddr
 * @return
 */
int	elfsh_set_segment_paddr(elfsh_Phdr *p, eresi_Addr paddr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_paddr = paddr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Documentation missing.
 * @param p
 * @return
 */
eresi_Addr	elfsh_get_segment_paddr(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_paddr));
}

/**
 * Documentation missing.
 * @param p
 * @param vaddr
 * @return
 */
int	elfsh_set_segment_vaddr(elfsh_Phdr *p, eresi_Addr vaddr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_vaddr = vaddr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Documentation missing.
 * @param p
 * @return
 */
eresi_Addr	elfsh_get_segment_vaddr(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_vaddr));
}

/**
 * Documentation missing.
 * @param p
 * @param type
 * @return
 */
int		elfsh_set_segment_type(elfsh_Phdr *p, eresi_Addr type)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_type = (elfsh_Word) type;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Documentation missing.
 * @param p
 * @return
 */
elfsh_Word	elfsh_get_segment_type(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_type));
}

/**
 * Documentation missing.
 * @param p
 * @param offset
 * @return
 */
int		elfsh_set_segment_offset(elfsh_Phdr *p, eresi_Addr offset)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_offset = (eresi_Off) offset;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Documentation missing.
 * @param p
 * @return
 */
eresi_Off	elfsh_get_segment_offset(elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

 if (!p)
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid NULL argument", -1);
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_offset));
}

/**
 * Retreive the 'range'th segment header of type 'type' 
 * @param file
 * @param type
 * @param range
 * @return
 */
elfsh_Phdr	*elfsh_get_segment_by_type(elfshobj_t	*file, 
					   int		type, 
					   int		range)
{
  elfsh_Phdr	*pht;
  int		index;
  int		max;
  int		cnt;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First PHT checks */
  pht = elfsh_get_pht(file, &max);
  if (NULL == pht)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get PHT", NULL);
  
  /* Simple find-by-type loop */
  for (cnt = index = 0; index < max; index++)
    if (elfsh_get_segment_type(pht + index) == type)
      {
	if (cnt == range)
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (&pht[index]));
	cnt++;
      }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to find PHDR by type", NULL);
}


/**
 * Change endianess of PHT 
 * @param p
 * @param byteorder
 * @param sz
 * @return
 */
void		elfsh_endianize_pht(elfsh_Phdr *p, char byteorder, uint16_t sz)
{
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (byteorder == ELFDATA2MSB) {
#elif __BYTE_ORDER == __BIG_ENDIAN
  if (byteorder == ELFDATA2LSB) {
#else
#error Unexpected __BYTE_ORDER !
#endif

  for (index = 0; index < sz; index++)
    {
      p->p_type   = swap32(p->p_type);
      p->p_offset = swaplong(p->p_offset);
      p->p_vaddr  = swaplong(p->p_vaddr);
      p->p_paddr  = swaplong(p->p_paddr);
      p->p_filesz = swaplong(p->p_filesz);
      p->p_memsz  = swaplong(p->p_memsz);
      p->p_flags  = swap32(p->p_flags);
      p->p_align  = swaplong(p->p_align);
      p++;
    }
  }
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/**
 * Return an array of program (segment) header 
 * @param file
 * @return
 */
int		elfsh_load_pht(elfshobj_t *file)
{
  int		size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      (char *)"Invalid NULL parameter", -1);

  else if (file->pht != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  else if (!file->hdr->e_phoff)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, (char *)"No PHT", -1);
  
  if (file->hdr->e_phoff > file->fstat.st_size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
 	 (char *)"PHT file offset is larger than the file itself", -1);
  
  size = file->hdr->e_phentsize * file->hdr->e_phnum;
  XSEEK(file->fd, file->hdr->e_phoff, SEEK_SET, -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,file->pht, size, -1);
  XREAD(file->fd, file->pht, size, -1);

  /* Deal with cross-endianess binaries */
  elfsh_endianize_pht(file->pht, 
		      file->hdr->e_ident[EI_DATA], 
		      file->hdr->e_phnum);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Find the parent segment for this section 
 * @param file
 * @param new
 * @return
 */
elfsh_Phdr	*elfsh_get_parent_segment(elfshobj_t *file, elfshsect_t *enew)
{
  elfsh_Phdr	*actual;
  int		index;
  eresi_Addr	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (NULL == enew || NULL == enew->parent || NULL == file ||
      (NULL == enew->parent->pht && elfsh_load_pht(enew->parent)) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Try to find in pht */
  for (index = 0, actual = enew->parent->pht; index < enew->parent->hdr->e_phnum;
       index++)
      if (INTERVAL(actual[index].p_vaddr, enew->shdr->sh_addr,
		 actual[index].p_vaddr + actual[index].p_memsz))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (actual + index));

  /* Fix the address we look for depending on the file type and context */
  addr = (elfsh_is_debug_mode() && !elfsh_section_is_runtime(enew) ? 
	  file->rhdr.base + enew->shdr->sh_addr : enew->shdr->sh_addr);
  
  /* Try to find in runtime pht */
  for (index = 0, actual = enew->parent->rpht; index < enew->parent->rhdr.rphtnbr;
       index++)
    {

      //printf("RPHT parent : Trying to match interval %08X <= %08X < %08X \n", 
      //actual[index].p_vaddr, addr, actual[index].p_vaddr + actual[index].p_memsz);

      if (INTERVAL(actual[index].p_vaddr, addr,
		   actual[index].p_vaddr + actual[index].p_memsz))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (actual + index));
    }
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to get parent PHDR", NULL);
}


/**
 * Tell if the section belongs to the segment or not 
 * @param new
 * @param p
 * @return
 */
int		elfsh_segment_is_parent(elfshsect_t *new, elfsh_Phdr *p)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* For SHT matchs */
  if (p->p_offset && new->shdr->sh_offset)
    {
      if (INTERVAL(p->p_offset, new->shdr->sh_offset, p->p_offset + p->p_filesz))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
    }

  /* Added for RSHT matchs */
  else if (new->shdr->sh_addr && 
	   INTERVAL(p->p_vaddr, new->shdr->sh_addr, p->p_vaddr + p->p_memsz))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Get the base virtual address for an object 
 * @param file
 * @return
 */
eresi_Addr	elfsh_get_object_baseaddr(elfshobj_t *file)
{
  int					nbr;
  unsigned int				index;
  eresi_Addr  vaddr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  if (elfsh_get_pht(file, &nbr) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot read PHT", -1);

  vaddr = (eresi_Addr) -1;
  for (index = 0; index < nbr; index++)
    if (file->pht[index].p_type == PT_LOAD && file->pht[index].p_vaddr < vaddr)
      vaddr = file->pht[index].p_vaddr;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_get_objtype(file->hdr) == ET_DYN ? 
		      vaddr + file->rhdr.base :  vaddr));
}


/**
 * Return a ptr on the program header table 
 * @param file
 * @param num
 * @return
 */
void	*elfsh_get_pht(elfshobj_t *file, int *num)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->pht == NULL && elfsh_load_pht(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to load PHT", NULL);
  if (num != NULL)
    *num = file->hdr->e_phnum;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->pht));
}

/**
 * Return a entry giving its parent and its index 
 * @param pht
 * @param index
 * @return
 */
elfsh_Phdr	*elfsh_get_pht_entry_by_index(elfsh_Phdr *pht, 
					      eresi_Addr index)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (pht + index));
}



/**
 * Create a new PHT entry 
 * @param t
 * @param a
 * @param z
 * @param al
 * @return
 */
elfsh_Phdr	elfsh_create_phdr(elfsh_Word t, 
				  eresi_Addr a, 
				  eresi_Off z, 
				  elfsh_Word al)
{
  elfsh_Phdr	new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  bzero(&new, sizeof(elfsh_Phdr));
  new.p_type   = t;
  new.p_vaddr  = new.p_paddr = a;
  new.p_filesz = new.p_memsz = z;
  new.p_align = al;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/**
 * Insert a PHT entry ondisk
 * Should be OK if called in last from section injection routine 
 * @param file
 * @param h
 * @return
 */
elfsh_Phdr	*elfsh_insert_phdr(elfshobj_t *file, elfsh_Phdr *h)
{
  elfshsect_t	*cur;
  elfsh_Phdr	*enew;
  elfsh_Phdr	*phdr;
  elfsh_Phdr	*curphdr;
  elfsh_SAddr	range;
  int		pagesize;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check if we extended the PHT already */
  if (elfsh_dynamic_file(file))
    {
      phdr = elfsh_get_segment_by_type(file, PT_PHDR, 0);
      if (!phdr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot find PT_PHDR segment",  NULL);
      
      if (!(phdr->p_filesz % elfsh_get_pagesize(file)) && 
	  (file->hdr->e_phnum + 1) * file->hdr->e_phentsize <= phdr->p_filesz)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__,enew, (file->hdr->e_phnum + 1) * elfsh_get_phentsize(file->hdr),
		 NULL);
	  memcpy(enew, file->pht, 
		 file->hdr->e_phnum * elfsh_get_phentsize(file->hdr));
	  goto end;
	}
    }

  /* Find the first executable PT_LOAD segment */
  range = 0;
  do
    {
      phdr = elfsh_get_segment_by_type(file, PT_LOAD, range);
      range++;
    }
  while (phdr && !elfsh_segment_is_executable(phdr));
  if (phdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot find +x PT_LOAD",  NULL);

  /* Copy beginning of new PHT */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,enew, (file->hdr->e_phnum + 1) * elfsh_get_phentsize(file->hdr), NULL);
  memcpy(enew, file->pht, file->hdr->e_phnum * elfsh_get_phentsize(file->hdr));

  /* Update it so that it covers the whole new PT_PHDR */
  pagesize = elfsh_get_pagesize(file);
  phdr = enew + (phdr - file->pht);
  phdr->p_filesz += pagesize;
  phdr->p_memsz  += pagesize;
  phdr->p_vaddr  -= pagesize;
  phdr->p_paddr  -= pagesize;

  /* Update all section file offsets */
  for (cur = file->sectlist; cur != NULL; cur = cur->next)
    if (cur->shdr->sh_offset)
      cur->shdr->sh_offset += elfsh_get_pagesize(file);

  /* Update all PHT file offsets */
  for (range = 0, curphdr = enew; 
       range < file->hdr->e_phnum; 
       range++, curphdr++)
    if (elfsh_get_segment_type(curphdr) == PT_PHDR)
      {
	curphdr->p_paddr  -= elfsh_get_pagesize(file);
	curphdr->p_vaddr  -= elfsh_get_pagesize(file);
	if (curphdr->p_filesz < elfsh_get_pagesize(file))
	  {
	    curphdr->p_filesz = 0;
	    curphdr->p_memsz  = 0;
	  }
	curphdr->p_filesz += elfsh_get_pagesize(file); 
	curphdr->p_memsz  += elfsh_get_pagesize(file); 
      }
    else if (curphdr != phdr && curphdr->p_offset)
      curphdr->p_offset += elfsh_get_pagesize(file);

  /* Fixup SHT file offset */
  file->hdr->e_shoff += elfsh_get_pagesize(file);

  /* Fixup injected-PHDR file offset given the chosen virtual address */
 end:
  if (h->p_vaddr)
    {
      cur = elfsh_get_parent_section(file, h->p_vaddr, &range);
      h->p_offset = cur->shdr->sh_offset + range;
    }
  memcpy(enew + file->hdr->e_phnum, h, elfsh_get_phentsize(file->hdr));

  /* Everything OK */
  file->hdr->e_phnum++;
  XFREE(__FILE__, __FUNCTION__, __LINE__,file->pht);
  file->pht = enew;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, enew + (file->hdr->e_phnum - 1));
}


/**
 * Remove a PHDR 
 * @param current
 * @param index
 * @return
 */
int		elfsh_remove_phdr(elfshobj_t *current, int index)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index < 0 || index >= current->hdr->e_phnum)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid index for PHDR", -1);

  if (!current->hdr->e_phnum)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "PHT is already empty", -1);

  if (current->hdr->e_phnum != 1)
    memcpy(current->pht + index, current->pht + index + 1, 
	   (current->hdr->e_phnum - index - 1) * current->hdr->e_phentsize);
  current->hdr->e_phnum--;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/**
 * @brief Set PHT entry rights from elfsh section mode.
 * @param mode ELFSH_CODE_INJECTION or ELFSH_DATA_INJECTION.
 * @return The same rights in PF_* flags format.
 */
int		elfsh_set_phdr_prot(unsigned int mode)
{
  elfsh_Word	flags;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  flags = PF_R | PF_W;
  //  if (mode == ELFSH_CODE_INJECTION)
  flags |= PF_X;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (flags));
}
