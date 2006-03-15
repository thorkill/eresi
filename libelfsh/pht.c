/*
** pht.c for libelfsh
** 
** Started on  Mon Feb 26 04:07:33 2001 mayhem
**
*/
#include "libelfsh.h"



/* 3 tiny functions to retreive the rights for a segment */
int	elfsh_segment_is_readable(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((p->p_flags & PF_R)));
}

int	elfsh_segment_is_writable(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((p->p_flags & PF_W)));
}

int	elfsh_segment_is_executable(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((p->p_flags & PF_X)));
}

/* Tiny write access functions for program headers */
int	elfsh_set_segment_flags(elfsh_Phdr *p, elfsh_Addr flags)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);

  p->p_flags = (elfsh_Word) flags;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

elfsh_Word	elfsh_get_segment_flags(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_flags));
}

int	elfsh_set_segment_align(elfsh_Phdr *p, elfsh_Addr align)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_align = (elfsh_Word) align;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

elfsh_Word	elfsh_get_segment_align(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_align));
}

int	elfsh_set_segment_memsz(elfsh_Phdr *p, elfsh_Addr memsz)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_memsz = (elfsh_Word) memsz;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

elfsh_Word	elfsh_get_segment_memsz(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_memsz));
}

int	elfsh_set_segment_filesz(elfsh_Phdr *p, elfsh_Addr filesz)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_filesz = (elfsh_Word) filesz;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

elfsh_Word	elfsh_get_segment_filesz(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_filesz));
}

int	elfsh_set_segment_paddr(elfsh_Phdr *p, elfsh_Addr paddr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_paddr = paddr;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

elfsh_Addr	elfsh_get_segment_paddr(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_paddr));
}

int	elfsh_set_segment_vaddr(elfsh_Phdr *p, elfsh_Addr vaddr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_vaddr = vaddr;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

elfsh_Addr	elfsh_get_segment_vaddr(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_vaddr));
}

int		elfsh_set_segment_type(elfsh_Phdr *p, elfsh_Addr type)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_type = (elfsh_Word) type;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

elfsh_Word	elfsh_get_segment_type(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_type));
}

int		elfsh_set_segment_offset(elfsh_Phdr *p, elfsh_Addr offset)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!p)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL argument", -1);
  p->p_offset = (elfsh_Off) offset;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

elfsh_Off	elfsh_get_segment_offset(elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 if (!p)
   ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid NULL argument", -1);
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (p->p_offset));
}



/* Retreive the 'range'th segment header of type 'type' */
elfsh_Phdr	*elfsh_get_segment_by_type(elfshobj_t	*file, 
					   int		type, 
					   int		range)
{
  elfsh_Phdr	*pht;
  int		index;
  int		max;
  int		cnt;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First PHT checks */
  pht = elfsh_get_pht(file, &max);
  if (NULL == pht)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get PHT", NULL);
  
  /* Simple find-by-type loop */
  for (cnt = index = 0; index < max; index++)
    if (elfsh_get_segment_type(pht + index) == type)
      {
	if (cnt == range)
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (&pht[index]));
	cnt++;
      }

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to find PHDR by type", NULL);
}


/* Change endianess of PHT */
void		elfsh_endianize_pht(elfsh_Phdr *p, char byteorder, uint16_t sz)
{
  int		index;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Return an array of program (segment) header */
int		elfsh_load_pht(elfshobj_t *file)
{
  int		size;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == file)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  else if (file->pht != NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  else if (!file->hdr->e_phoff)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "No PHT", -1);
  
  if (file->hdr->e_phoff > file->fstat.st_size)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
 	 "PHT file offset is larger than the file itself", -1);
  
  size = file->hdr->e_phentsize * file->hdr->e_phnum;
  XSEEK(file->fd, file->hdr->e_phoff, SEEK_SET, -1);
  XALLOC(file->pht, size, -1);
  XREAD(file->fd, file->pht, size, -1);

  /* Deal with cross-endianess binaries */
  elfsh_endianize_pht(file->pht, 
		      file->hdr->e_ident[EI_DATA], 
		      file->hdr->e_phnum);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Find the parent segment for this section */
elfsh_Phdr	*elfsh_get_parent_segment(elfshobj_t *file, elfshsect_t *new)
{
  elfsh_Phdr	*actual;
  int		index;
  elfsh_Addr	addr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (NULL == new || NULL == new->parent || NULL == file ||
      (NULL == new->parent->pht && elfsh_load_pht(new->parent)) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Try to find in pht */
  for (index = 0, actual = new->parent->pht; index < new->parent->hdr->e_phnum;
       index++)
      if (INTERVAL(actual[index].p_vaddr, new->shdr->sh_addr,
		 actual[index].p_vaddr + actual[index].p_memsz))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (actual + index));

  /* Fix the address we look for depending on the file type and context */
  addr = (elfsh_is_debug_mode() && !elfsh_section_is_runtime(new) ? 
	  file->rhdr.base + new->shdr->sh_addr : new->shdr->sh_addr);
  
  /* Try to find in runtime pht */
  for (index = 0, actual = new->parent->rpht; index < new->parent->rhdr.rphtnbr;
       index++)
    {

      //printf("RPHT parent : Trying to match interval %08X <= %08X < %08X \n", 
      //actual[index].p_vaddr, addr, actual[index].p_vaddr + actual[index].p_memsz);

      if (INTERVAL(actual[index].p_vaddr, addr,
		   actual[index].p_vaddr + actual[index].p_memsz))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (actual + index));
    }
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to get parent PHDR", NULL);
}


/* Tell if the section belongs to the segment or not */
int		elfsh_segment_is_parent(elfshsect_t *new, elfsh_Phdr *p)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* For SHT matchs */
  if (p->p_offset && new->shdr->sh_offset)
    {
      if (INTERVAL(p->p_offset, new->shdr->sh_offset, p->p_offset + p->p_filesz))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
    }

  /* Added for RSHT matchs */
  else if (new->shdr->sh_addr && 
	   INTERVAL(p->p_vaddr, new->shdr->sh_addr, p->p_vaddr + p->p_memsz))
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Get the base virtual address for an object */
elfsh_Addr	elfsh_get_object_baseaddr(elfshobj_t *file)
{
  u_int		nbr;
  u_int		index;
  elfsh_Addr   	vaddr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  if (elfsh_get_pht(file, &nbr) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot read PHT", -1);

  vaddr = (elfsh_Addr) -1;
  for (index = 0; index < nbr; index++)
    if (file->pht[index].p_type == PT_LOAD && file->pht[index].p_vaddr < vaddr)
      vaddr = file->pht[index].p_vaddr;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_get_objtype(file->hdr) == ET_DYN ? 
		      vaddr + file->rhdr.base :  vaddr));
}


/* Return a ptr on the program header table */
void	*elfsh_get_pht(elfshobj_t *file, int *num)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->pht == NULL && elfsh_load_pht(file) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to load PHT", NULL);
  if (num != NULL)
    *num = file->hdr->e_phnum;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->pht));
}

/* Return a entry giving its parent and its index */
elfsh_Phdr	*elfsh_get_pht_entry_by_index(elfsh_Phdr *pht, 
					      elfsh_Addr index)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (pht + index));
}



/* Create a new PHT entry */
elfsh_Phdr	elfsh_create_phdr(elfsh_Word t, 
				  elfsh_Addr a, 
				  elfsh_Off z, 
				  elfsh_Word al)
{
  elfsh_Phdr	new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  bzero(&new, sizeof(elfsh_Phdr));
  new.p_type   = t;
  new.p_vaddr  = new.p_paddr = a;
  new.p_filesz = new.p_memsz = z;
  new.p_align = al;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/* Insert a PHT entry ondisk */
/* Should be OK if called in last from section injection routine */
elfsh_Phdr	*elfsh_insert_phdr(elfshobj_t *file, elfsh_Phdr *h)
{
  elfshsect_t	*cur;
  elfsh_Phdr	*new;
  elfsh_Phdr	*phdr;
  elfsh_Phdr	*curphdr;
  elfsh_SAddr	range;
  int		pagesize;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check if we extended the PHT already */
  if (elfsh_dynamic_file(file))
    {
      phdr = elfsh_get_segment_by_type(file, PT_PHDR, 0);
      if (!phdr)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot find PT_PHDR segment",  NULL);
      
      if (!(phdr->p_filesz % elfsh_get_pagesize(file)) && 
	  (file->hdr->e_phnum + 1) * file->hdr->e_phentsize <= phdr->p_filesz)
	{
	  XALLOC(new, (file->hdr->e_phnum + 1) * elfsh_get_phentsize(file->hdr),
		 NULL);
	  memcpy(new, file->pht, 
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
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot find +x PT_LOAD",  NULL);

  /* Copy beginning of new PHT */
  XALLOC(new, (file->hdr->e_phnum + 1) * elfsh_get_phentsize(file->hdr), NULL);
  memcpy(new, file->pht, file->hdr->e_phnum * elfsh_get_phentsize(file->hdr));

  /* Update it so that it covers the whole new PT_PHDR */
  pagesize = elfsh_get_pagesize(file);
  phdr = new + (phdr - file->pht);
  phdr->p_filesz += pagesize;
  phdr->p_memsz  += pagesize;
  phdr->p_vaddr  -= pagesize;
  phdr->p_paddr  -= pagesize;

  /* Update all section file offsets */
  for (cur = file->sectlist; cur != NULL; cur = cur->next)
    if (cur->shdr->sh_offset)
      cur->shdr->sh_offset += elfsh_get_pagesize(file);

  /* Update all PHT file offsets */
  for (range = 0, curphdr = new; 
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
  memcpy(new + file->hdr->e_phnum, h, elfsh_get_phentsize(file->hdr));

  /* Everything OK */
  file->hdr->e_phnum++;
  XFREE(file->pht);
  file->pht = new;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, new + (file->hdr->e_phnum - 1));
}


/* Remove a PHDR */
int		elfsh_remove_phdr(elfshobj_t *current, int index)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index < 0 || index >= current->hdr->e_phnum)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid index for PHDR", -1);

  if (!current->hdr->e_phnum)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "PHT is already empty", -1);

  if (current->hdr->e_phnum != 1)
    memcpy(current->pht + index, current->pht + index + 1, 
	   (current->hdr->e_phnum - index - 1) * current->hdr->e_phentsize);
  current->hdr->e_phnum--;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
