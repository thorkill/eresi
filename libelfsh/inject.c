/**
 * @file inject.c
 * @ingroup libelfsh
** inject.c for libelfsh
** 
** Contains all section injection API that can be used directly by the user
** 
** Started on  Thu Jun 09 00:12:42 2005 mm
** 
**
** $Id: inject.c,v 1.15 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"




/**
 * Insert a new section at the first place in the executable PT_LOAD
 * This function is not e2dbg safe and should only be used for ondisk files
 * This function is ET_DYN-PaX-pie-hardened-gentoo-safe 
 * @param file
 * @param sect
 * @param hdr
 * @param data
 * @param mod
 * @return
 */
int		elfsh_insert_code_section(elfshobj_t	*file,
					  elfshsect_t	*sect,
					  elfsh_Shdr	hdr,
					  void		*data,
					  unsigned int		mod)
{
  elfshsect_t	*first;
  elfsh_Phdr	*phdr;
  elfsh_Phdr	*cur;
  int		range;
  char		*rdata;
  unsigned int		rsize;
  unsigned int		index;
  int		err;
  elfshsect_t	*relsect;
  int		check;
  eresi_Addr	entrypoint;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Cannot get SHT", -1);

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
		      "Cannot find +x PT_LOAD",  -1);
  
  /* If the executable segment starts at low address, use 
     alternative code section injection */
  if (phdr->p_vaddr <= ELFSH_SPARC_LOWADDR && 
      (elfsh_get_archtype(file) == ELFSH_ARCH_SPARC32 ||
       elfsh_get_archtype(file) == ELFSH_ARCH_SPARC64))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       (elfsh_insert_code_section_up(file, sect, hdr, data, mod)));

  /* FIXME: just a try on recent ld */
  /* So that file offset and vaddr and aligned */
  else if (elfsh_get_archtype(file) == ELFSH_ARCH_SPARC32 ||
	   elfsh_get_archtype(file) == ELFSH_ARCH_SPARC64)
      elfsh_set_segment_align(phdr, elfsh_get_pagesize(file));     
  
  /* Find the first allocatable section */
  first = file->sectlist;
  while (!first->shdr->sh_addr)
    first = first->next;

#if	__DEBUG_RELADD__	      
  printf("[DEBUG_RELADD] Guard section found : %s (first->next = %p) \n", first->name, first->next);
#endif

  if (first == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find guard section", -1);

  /* Pad the new section if needed */
  /*  I KEEP OLD METHOD ON COMMENT (IN CASE)
  if (mod && (hdr.sh_size % mod))
    {
      rsize = hdr.sh_size + mod - (hdr.sh_size % mod);
      XALLOC(__FILE__, __FUNCTION__, __LINE__,rdata, rsize, -1);
      if (data)
	memcpy(rdata, data, hdr.sh_size);
      hdr.sh_size = rsize;
      data = rdata;
    }
  */

  /* New method that how we should do no ? */
  check = (phdr->p_vaddr - phdr->p_offset) & (phdr->p_align - 1);
  if (check != 0)
    {
      rsize = hdr.sh_size + (check + 1);	
      XALLOC(__FILE__, __FUNCTION__, __LINE__,rdata, rsize, -1);
      if (data)
	memcpy(rdata, data, hdr.sh_size);
      hdr.sh_size = rsize;
      data = rdata;
    }

#ifdef __BEOS__
  if (first->shdr->sh_addr < hdr.sh_size)
     printf("Trap Error: sh_addr < sh_size !\n");
#endif
  
  /* Extend the first loadable segment at low addresses */
  if (elfsh_get_objtype(file->hdr) != ET_DYN)
    hdr.sh_addr = first->shdr->sh_addr - hdr.sh_size;
  else
    hdr.sh_addr = first->shdr->sh_addr;
  hdr.sh_offset = first->shdr->sh_offset;
  
  /* Fixup the file offset */
  phdr->p_filesz += hdr.sh_size;
  phdr->p_memsz  += hdr.sh_size;

  /* On ET_DYN, we put the new section at the end, so we dont do this */
  if (elfsh_get_objtype(file->hdr) != ET_DYN)
    {
      phdr->p_vaddr  -= hdr.sh_size;
      phdr->p_paddr  -= hdr.sh_size;
    }

  entrypoint = elfsh_get_entrypoint(file->hdr);

  /* Fixup file offset for all loadable segments and fixup PHDR base vaddr */
  for (range = 0, cur = file->pht; range < file->hdr->e_phnum; range++)
    {
      /* That's how we shift on ET_EXEC */
      if (cur[range].p_type == PT_PHDR && 
	  elfsh_get_objtype(file->hdr) != ET_DYN)
	{
	  cur[range].p_vaddr -= hdr.sh_size;
	  cur[range].p_paddr -= hdr.sh_size;
	}
      else if (cur + range != phdr && cur[range].p_offset >= hdr.sh_offset)
	{
	  cur[range].p_offset += hdr.sh_size;

	  /* That's how we shift the address space on ET_DYN */
	  if (elfsh_get_objtype(file->hdr) == ET_DYN)
	    {
	      cur[range].p_vaddr += hdr.sh_size;
	      cur[range].p_paddr += hdr.sh_size;
	    }
	}	  
    }

  /* Inject our section with the associated header */
  index = first->index;
  if (elfsh_insert_shdr(file, hdr, index, sect->name, 1) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to insert shdr", -1);
  
  if (elfsh_add_section(file, sect, index, data,
			elfsh_get_objtype(file->hdr) == ET_DYN ? 
			ELFSH_SHIFTING_COMPLETE : ELFSH_SHIFTING_PARTIAL) < 0)
			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to add section", -1);

  /* Shift stuff in the ET_DYN */
  if (elfsh_get_objtype(file->hdr) == ET_DYN)
    {
      for (index = 0; 1; index++)
	{
	  relsect = elfsh_get_reloc(file, (eresi_Addr) index, NULL);
	  if (!relsect)
	    {
	      if (!index)
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				  "Not a single relo table found", -1);
	      break;
	    }
	  err = elfsh_shift_ia32_relocs(file, hdr.sh_size, relsect, 
					sect->shdr->sh_addr + sect->shdr->sh_size);
	  if (err < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Problem while shifting relocs", -1);
	}

      err = elfsh_shift_symtab(file, sect->shdr->sh_addr, hdr.sh_size);
      if (err < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot shift symtab in ET_DYN", -1);
      
      err = elfsh_shift_dynsym(file, sect->shdr->sh_addr, hdr.sh_size);
      if (err < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot shift dynsym in ET_DYN", -1);
      
      elfsh_update_dynsym_shidx(file, sect->index - 1, 1);
      elfsh_update_symtab_shidx(file, sect->index - 1, 1);

      if (elfsh_shift_dynamic(file, sect->shdr->sh_size) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Cannot shift dynamic in ET_DYN", -1);

      if (elfsh_shift_got(file, sect->shdr->sh_size, 
			  ELFSH_SECTION_NAME_GOT) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Cannot shift .got in ET_DYN", -1);
      
      if (elfsh_shift_got(file, sect->shdr->sh_size,
			  ELFSH_SECTION_NAME_GOTPLT) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Cannot shift .got.plt in ET_DYN", -1);
      
      /* ALTGOT section is not present in unmodified binaries so its not a fatal error */
      elfsh_shift_got(file, sect->shdr->sh_size, ELFSH_SECTION_NAME_ALTGOT);
	
      if (elfsh_shift_dtors(file, sect->shdr->sh_size) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Cannot shift dtors in ET_DYN", -1);
      
      if (elfsh_shift_ctors(file, sect->shdr->sh_size) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Cannot shift ctors in ET_DYN", -1);

      elfsh_set_entrypoint(file->hdr, elfsh_get_entrypoint(file->hdr) + sect->shdr->sh_size);
    }


  /* Inject the SECT symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to insert section symbol", -1);


  /* Okay ! */
  sect->phdr = phdr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->index));
}



/**
 * Insert a new section at the first place in the executable PT_LOAD 
 * WORK IN PROGRESS DO NOT USE IT FOR NOW
 * This function is not e2dbg safe and should only be used for ondisk files 
 *
 * @param file
 * @param sect
 * @param hdr
 * @param data
 * @param mod
 * @return
 */
int		elfsh_insert_code_section_up(elfshobj_t		*file,
					     elfshsect_t	*sect,
					     elfsh_Shdr		hdr,
					     void		*data,
					     unsigned int	        mod)
{
  elfshsect_t	*last;
  elfsh_Phdr	*phdr;
  elfsh_Phdr	*phdr_code;
  elfsh_Phdr	*phdr_data;
  elfsh_Phdr	*cur;
  int		range;
  char		*rdata;
  unsigned int		rsize;
  unsigned int		alignedsize;
  unsigned int		alignedsize2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get SHT", -1);

  /* Find both PT_LOAD segment */
  phdr_code = phdr_data = NULL;
  range = 0;
  do
    {
      phdr = elfsh_get_segment_by_type(file, PT_LOAD, range);
#if	__DEBUG_RELADD__	      
      printf("Found PT_LOAD at range %u \n", range);
#endif
      if (!phdr)
	break;
      range++;
      if (elfsh_segment_is_executable(phdr))
	{
	  if (!elfsh_segment_is_writable(phdr))
	    phdr_code = phdr;
	  else
	    phdr_data = phdr;
	}
    }
  while (phdr);
  
  if (phdr_code == NULL || phdr_data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find 2 PT_LOAD",  -1);
  
  /* Find the last section of the executable PT_LOAD */
  last = file->sectlist;
  while (!last->phdr || last->phdr->p_vaddr < phdr_data->p_vaddr)
    last = last->next;
  if (last == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find last +X section", -1);

  last = last->prev;
  
  /* Pad the new section if needed */
  if (mod && (hdr.sh_size % mod))
    {
      rsize = hdr.sh_size + mod - (hdr.sh_size % mod);
      XALLOC(__FILE__, __FUNCTION__, __LINE__,rdata, rsize, -1);
      if (data)
	memcpy(rdata, data, hdr.sh_size);
      hdr.sh_size = rsize;
      data = rdata;
    }

#ifdef __BEOS__
  if (last->shdr->sh_addr < hdr.sh_size)
     printf("Trap Error: sh_addr < sh_size !\n");
#endif

  /* Extend the executable segment at high addresses */
  hdr.sh_addr = last->shdr->sh_addr + last->shdr->sh_size;
  hdr.sh_offset = last->shdr->sh_offset + last->shdr->sh_size;
  phdr_code->p_filesz += hdr.sh_size;
  phdr_code->p_memsz  += hdr.sh_size;

  /* Fixup file offset for all loadable segments and fixup PHDR base vaddr */
  alignedsize = alignedsize2 = 0;
  elfsh_set_segment_align(phdr_code, elfsh_get_pagesize(file));

  /* This section injection requires strong alignment constraints */
  for (range = 0, cur = file->pht; range < file->hdr->e_phnum; range++)
    if (cur != phdr_code && cur[range].p_offset >= last->shdr->sh_offset)
      {
	cur[range].p_offset += hdr.sh_size + alignedsize2; // + alignedsize2;
	if (elfsh_get_segment_type(cur + range) == PT_LOAD)
	  {
	    elfsh_set_segment_align(cur + range, elfsh_get_pagesize(file));

	    /* Align on file offset */
	    /*
	    if (cur[range].p_offset % elfsh_get_pagesize(file))
	      {
		printf("Found misaligned foffset in phdr index %u (diff = %u) \n", 
		       range, cur[range].p_offset % elfsh_get_pagesize(file));
		alignedsize = elfsh_get_pagesize(file) - (cur[range].p_offset % elfsh_get_pagesize(file));
		cur[range].p_offset += alignedsize;
	      }
	    else
	      printf("foffset align test not entered\n");
	    */

	    /* Align on virtual addr vs file offset */
	    if ((cur[range].p_vaddr - cur[range].p_offset) % elfsh_get_pagesize(file))
	      {
#if	__DEBUG_RELADD__	      
		printf("Found misaligned off/addr in phdr index %u (diff = %u) \n", 
		       range, (cur[range].p_vaddr - cur[range].p_offset) % elfsh_get_pagesize(file));
#endif
		alignedsize2 = elfsh_get_pagesize(file) - ((cur[range].p_vaddr - cur[range].p_offset) % elfsh_get_pagesize(file));
		cur[range].p_offset += alignedsize2;
	      }
#if	__DEBUG_RELADD__	      
	    else
	      printf("vaddr-foffset align test not entered\n");
#endif
	  }
      }
  
  /* Inject our section with the associated header */
  if (elfsh_insert_shdr(file, hdr, last->index + 1, sect->name, 1) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to insert shdr", -1);
  
  if (elfsh_add_section(file, sect, last->index + 1,
			data, ELFSH_SHIFTING_PARTIAL) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to add section", -1);

  /* Inject the SECT symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to insert section symbol", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->index));
}





/**
 * Insert a data section in the object 
 * This function is not e2dbg safe and should only be used with ondisk files
 * Use elfsh_insert_runtime_section() for runtime injections 
 *
 * @param file
 * @param sect
 * @param hdr
 * @param data
 * @return
 */
int		elfsh_insert_data_section(elfshobj_t	*file,
					  elfshsect_t	*sect,
					  elfsh_Shdr	hdr,
					  void		*data)
{
  elfshsect_t	*last;
  void		*rdata;
  unsigned int		pad = 0;
  elfsh_Phdr	*phdr = NULL, *phdr2 = NULL;
  unsigned int		range;


  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get SHT", -1);

  /* Insert the bss physically in the file if not already done */
  if (elfsh_fixup_bss(file) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot fixup .bss", -1);

  /* Find the PHDR */
  range = 0;
  do
    {
      phdr2 = phdr;
      phdr = elfsh_get_segment_by_type(file, PT_LOAD, range);
      range++;
    }
  while (phdr);
  phdr = phdr2;
  if (phdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find last PT_LOAD",  -1);
  
  /* Iterate and get the real last mapped section */
  last = file->sectlist;										
  while (last->next != NULL && last->next->shdr->sh_addr != NULL)					
    last = last->next;											
  last->phdr = phdr;											
													
  /* Avoid unaligned accesses */									
  if ((last->shdr->sh_addr + last->shdr->sh_size) % sizeof(eresi_Addr))
    pad = sizeof(eresi_Addr) - ((last->shdr->sh_addr + last->shdr->sh_size) % sizeof(eresi_Addr));
      
#if __DEBUG_RELADD__
  if (pad)
    printf("[DEBUG_RELADD] Small gap of %u bytes between %s and %s \n", 
	   pad, last->name, sect->name);
#endif

  /* Extend the segment to insert the new section */
  hdr.sh_addr = last->shdr->sh_addr + last->shdr->sh_size + pad;
  hdr.sh_offset = last->shdr->sh_offset + last->shdr->sh_size + pad;
  last->phdr->p_filesz += hdr.sh_size + pad;
  last->phdr->p_memsz  += hdr.sh_size + pad;
  
  /* Copy the data */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,rdata, hdr.sh_size, -1);
  if (data)
    memcpy(rdata, data, hdr.sh_size);

  /* Inject our section with the associated header */
  if (elfsh_insert_shdr(file, hdr, last->index + 1, sect->name, 1) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot insert shdr", -1);

  if (elfsh_add_section(file, sect, last->index + 1,
			rdata, ELFSH_SHIFTING_COMPLETE) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot add section", -1);

  /* Next pointers will be updated, we need them for padding */
  last = elfsh_get_section_by_name(file, sect->name, 0, 0, 0);
  if (last == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive injected section", -1);

  /* Inject the SECT symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot insert section symbol", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->index));
}



/**
 * 
 * This function need to be modularized so that it can serves for runtime mapping
 * and also static file mapping in new PT_LOAD. Since thats what this function do
 * it shouldnt be a problem. Just a thing to check : make sure the (real) PHT can 
 * be extended from this function, because PHT extension uses another injection 
 * internaly so I hope nothing is fucked up -mm
 *
 * Make sure also to use a dedicated base addr for the new section if you re
 * debugging heap sensible code. 
 *
 * Runtime injection : the standard process injection 
 *
 * @param file
 * @param sect
 * @param hdr
 * @param data
 * @param mode
 * @param mod
 * @return
 */
int		elfsh_insert_runtime_section(elfshobj_t	 *file,
					     elfshsect_t *sect,
					     elfsh_Shdr	 hdr,
					     void	 *data,
					     int	 mode,
					     unsigned int	 mod)
{
  elfsh_Phdr	phdr;
  unsigned int		rsize;
  char		*rdata;
  int		range;
  /*
  elfshsect_t	*bss;
  */


  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Grab the BSS */
  /*
  bss = elfsh_get_section_by_name(file,
				  ELFSH_SECTION_NAME_BSS,
				  NULL, NULL, NULL);
  if (!bss)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot find ondisk BSS", -1);
  while (bss->next)
    bss = bss->next;
  */

  /* Pad the new section if needed */
  if (mod && (hdr.sh_size % mod))
    {
      rsize = hdr.sh_size + mod - (hdr.sh_size % mod);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, rdata, rsize, -1);
      if (data)
	memcpy(rdata, data, hdr.sh_size);
      else
	memset(rdata, 0x00, hdr.sh_size);
      hdr.sh_size = rsize;
      data = rdata;
    }

  /* If someone provided a NULL data pointer, provide a zero'd zone */
  else
    {
      rsize = hdr.sh_size;
      if (!data)
	XALLOC(__FILE__, __FUNCTION__, __LINE__, data, rsize, -1);
    }

  /* Create and inject the new PT_LOAD in runtime */
  phdr = elfsh_create_phdr(PT_LOAD, 0, rsize, mod);

  /* In runtime static binary injection, we need a safe p_vaddr each time we call this function */
  phdr.p_flags = elfsh_set_phdr_prot(mode);
  phdr.p_vaddr = elfsh_runtime_map(&phdr); 
  if (phdr.p_vaddr == ELFSH_INVALID_ADDR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot runtime map", -1);

  /* Copy the data in memory */
#if	__DEBUG_RUNTIME__	      
  printf("[DEBUG_RUNTIME] Writing data (%p) in memory at addr %08X (pid = %hu) \n", 
	 data, phdr.p_vaddr, getpid());
#endif

#if defined(KERNSH)
  if(kernsh_is_mem_mode())
    {
      kernsh_writemem(phdr.p_vaddr, data, rsize);
    }
  else
    {
      memcpy((void *) phdr.p_vaddr, data, rsize);
    }
#else
  memcpy((void *) phdr.p_vaddr, data, rsize);
#endif

  /* Modify some ondisk information */
  phdr.p_paddr  = phdr.p_vaddr;
  hdr.sh_addr   = phdr.p_vaddr;
  //hdr.sh_offset = bss->shdr->sh_offset + bss->shdr->sh_size;

#if	__DEBUG_RUNTIME__	      
  printf("[DEBUG_RUNTIME] Runtime injection of %s section data ! \n", sect->name);
#endif



  /* Insert the new program header in the runtime PHDR */
  /* XXX: insert in real PHT if doing non-runtime _static file_ injection */
  /* Should already work on runtime static binary modification */
  /* Use elfsh_insert_phdr in this static case */
  /* After modification, should be OK at least on x86 */
  //phdr.p_offset = sect->shdr->sh_offset;  
  sect->phdr = elfsh_insert_runtime_phdr(file, &phdr); 
  if (!sect->phdr)
    {
      elfsh_runtime_unmap(&phdr); 
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			    "Cannot insert RPHT entry", -1);
    }      

  /* Synchronize the ondisk perspective */
  range = elfsh_insert_runtime_shdr(file, hdr, file->rhdr.rshtnbr, sect->name, 1);
  if (range < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot insert shdr", -1);

  if (elfsh_add_runtime_section(file, sect, range, data) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot add section", -1);
  
  /* Inject the SECT symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot insert sectsym", -1);

#if	__DEBUG_RUNTIME__	      
  printf("[DEBUG_RUNTIME] Runtime injected %s at addr " XFMT "! \n", 
	 sect->name, sect->shdr->sh_addr);
#endif

  /* Always force file offset 0 for runtime sections */
  sect->shdr->sh_offset = 0;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->index));
}



/**
 * Static binary injection : section injection for static binaries 
 *
 * @param file
 * @param sect
 * @param hdr
 * @param data
 * @param mode
 * @param mod
 * @return
 */
int		elfsh_insert_static_section(elfshobj_t	 *file,
					    elfshsect_t  *sect,
					    elfsh_Shdr	 hdr,
					    void	 *data,
					    int		 mode,
					    unsigned int	 mod)
{
  elfsh_Phdr	phdr;
  elfsh_Phdr	*curphdr;
  unsigned int		rsize;
  char		*rdata;
  elfshsect_t	*lastsect;
  elfshsect_t	*cursect;
  unsigned int	phtnew;
  unsigned int  totsize;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fixup BSS if not already done */
  lastsect = elfsh_fixup_bss(file);
  if (lastsect == NULL)
    lastsect = file->sectlist; 
  
  /* Grab the last mapped section */
  while (lastsect->next && lastsect->next->shdr->sh_addr)
    lastsect = lastsect->next; 
  

#if	__DEBUG_STATIC__
  printf("[DEBUG_STATIC] Found last section for static injection : %s \n", lastsect->name);
#endif

  /* Pad the new section if needed */
  if (mod && (hdr.sh_size % mod))
    {
      rsize = hdr.sh_size + mod - (hdr.sh_size % mod);
      XALLOC(__FILE__, __FUNCTION__, __LINE__,rdata, rsize, -1);
      if (data)
	memcpy(rdata, data, hdr.sh_size);
      hdr.sh_size = rsize;
      data = rdata;
    }
  else
    rsize = hdr.sh_size;

  /* Find an existing PT_LOAD for loading the new section, if possible */
  phtnew = 0;
  curphdr = &file->pht[file->hdr->e_phnum - 1];
  for (totsize = 0, cursect = file->sectlist; cursect; cursect = cursect->next)
    if (cursect->phdr == curphdr)
      totsize += cursect->shdr->sh_size;

  /* If the latest injected PT_LOAD is not big enough or does not match the rights create a new one */
  if (curphdr->p_type != PT_LOAD || totsize + rsize > curphdr->p_filesz) // ||
      /* (elfsh_segment_is_executable(curphdr) && elfsh_get_section_writableflag(&hdr)) ||
	 (elfsh_segment_is_writable(curphdr) && elfsh_get_section_execflag(&hdr))) */
    phtnew = 1;
  else
    {
      curphdr->p_filesz += rsize;
      curphdr->p_memsz  += rsize;
    }

  /* Case where we inject in a new segment */
  if (phtnew)
    {
      
      /* Create and inject the new PT_LOAD */
      phdr = elfsh_create_phdr(PT_LOAD, 0, elfsh_get_pagesize(file), mod);
      phdr.p_flags = elfsh_set_phdr_prot(mode);
      phdr.p_vaddr = lastsect->shdr->sh_addr + lastsect->shdr->sh_size;
      if (phdr.p_vaddr % elfsh_get_pagesize(file)) 
	phdr.p_vaddr += elfsh_get_pagesize(file) - phdr.p_vaddr % elfsh_get_pagesize(file);
      
      /* Modify some ondisk information */
      phdr.p_paddr  = phdr.p_vaddr;
      hdr.sh_addr   = phdr.p_vaddr;
      hdr.sh_offset = lastsect->shdr->sh_offset + lastsect->shdr->sh_size;
      
      /* align section's foffset */
      if (hdr.sh_offset % elfsh_get_pagesize(file))
	hdr.sh_offset += elfsh_get_pagesize(file) - hdr.sh_offset %  elfsh_get_pagesize(file);
      
#if __DEBUG_STATIC__
      printf("[DEBUG_STATIC] Static injection of %s section data in NEW PHDR! \n", sect->name);
#endif
    }

  /* Case where we reuse a PT_LOAD */
  else
    {
      hdr.sh_addr   = curphdr->p_vaddr  + curphdr->p_memsz - rsize;
      hdr.sh_offset = curphdr->p_offset + curphdr->p_memsz - rsize;

#if __DEBUG_STATIC__
      printf("[DEBUG_STATIC] Static injection of %s section data with EXISTING PHDR! \n", sect->name);
#endif
    }

  /* Synchronize the ondisk perspective */
  if (elfsh_insert_shdr(file, hdr, lastsect->index + 1, sect->name, 1) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot insert shdr", -1);

  if (elfsh_add_section(file, sect, lastsect->index + 1,
			data, ELFSH_SHIFTING_PARTIAL) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot add section", -1);

  /* Inject the SECT symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot insert sectsym", -1);

  /* Insert the new program header in the runtime PHDR */
  if (phtnew)
    {
      phdr.p_offset = sect->shdr->sh_offset;
      sect->phdr = elfsh_insert_phdr(file, &phdr); 
      if (!sect->phdr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot insert PHT entry", -1);
    }
  else
    sect->phdr = curphdr;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->index));
}





/**
 * Insert a mapped section in the object
 * This function is e2dbg safe 
 *
 * @param file
 * @param sect
 * @param hdr
 * @param data
 * @param mode
 * @param modulo
 * @return
 */
int		elfsh_insert_mapped_section(elfshobj_t	*file,
					    elfshsect_t *sect,
					    elfsh_Shdr	hdr,
					    void	*data,
					    int		mode,
					    unsigned int	modulo)
{
  int		err;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  mode = (char) mode;
  if (file == NULL || sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  /* Runtime injection in memory */
  if (elfsh_debugger_present())
    {
      err = elfsh_insert_runtime_section(file, sect, hdr, data, mode, modulo);
      if (err < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot injection runtime section", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Static binary injection ondisk */
  if (elfsh_static_file(file))
    {
      err = elfsh_insert_static_section(file, sect, hdr, data, mode, modulo);
      if (err < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot injection static section", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Dynamic binary injection ondisk */
  switch (mode)
    {

    case ELFSH_CODE_INJECTION:
      err = elfsh_insert_code_section(file, sect, hdr, data, modulo);
      if (err < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Cannot perform ondisk code injection", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

    case ELFSH_DATA_INJECTION:
      err = elfsh_insert_data_section(file, sect, hdr, data);
      if (err < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Cannot perform ondisk data injection", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown mode", -1);
    }
}


/**
 * Insert a non-mapped section in the object 
 *
 * @param file
 * @param sect
 * @param hdr
 * @param data
 * @return
 */
int		elfsh_insert_unmapped_section(elfshobj_t	*file,
					      elfshsect_t	*sect,
					      elfsh_Shdr	hdr,
					      void		*data)
{
  elfshsect_t	*s;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (file == NULL || sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  /* Sanity checks */
  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot get SHT", -1);

  /* Get the last current section */
  s = elfsh_get_section_by_index(file, file->hdr->e_shnum - 1, NULL, NULL);
  if (s == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot get last section by index", -1);

  /* Fix the file offset for the new section */
  hdr.sh_offset = s->shdr->sh_offset + s->shdr->sh_size;

  /* Make sure the new unmapped section do not overlap SHT */
  if (hdr.sh_offset <= file->hdr->e_shoff &&
      hdr.sh_offset + hdr.sh_size >= file->hdr->e_shoff)
    hdr.sh_offset = file->hdr->e_shoff + (file->hdr->e_shnum *
					  file->hdr->e_shentsize);

  /* Inject the new section and the associated header */
  if (elfsh_insert_shdr(file, hdr, file->hdr->e_shnum,
			sect->name, 1) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot insert shdr", -1);

  if (elfsh_add_section(file, sect, file->hdr->e_shnum - 1,
			data, ELFSH_SHIFTING_COMPLETE) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot add section", -1);

  /* Inject the symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot insert section symbol", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->index));
}


/**
 * Insert a section in the object
 * This function is e2dbg safe
 *
 * @param file
 * @param name
 * @param data
 * @param mode
 * @param size
 * @param mod
 * @return
 */
elfshsect_t*		elfsh_insert_section(elfshobj_t	 *file,
					     char	 *name, 
					     char	*data,
					     char	 mode, 
					     unsigned int	 size,
					     unsigned int	 mod)
{
  elfshsect_t	*sect;
  elfsh_Shdr	hdr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (data == NULL)
    XALLOC(__FILE__, __FUNCTION__, __LINE__, data, size, NULL);

  /* Creation new section */
  sect = elfsh_create_section(name);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot create section", NULL);

  /* Preliminary checks */
  if (file == NULL || sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  switch (mode)
    {

    case ELFSH_CODE_INJECTION:
      hdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC,
			      0, 0, size, 0, 0, 0, 0);
      if (elfsh_insert_mapped_section(file, sect, hdr, data, mode, mod) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot insert mapped code section", NULL);
      break;

    case ELFSH_DATA_INJECTION:
      hdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_WRITE | SHF_ALLOC, 
			      0, 0, size, 0, 0, 0, 0);
      if (elfsh_insert_mapped_section(file, sect, hdr, data, mode, mod) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot insert mapped data section", NULL);
      break;

    case ELFSH_UNMAPPED_INJECTION:
      hdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, size, 0, 0, 0, 0);
      if (elfsh_insert_unmapped_section(file, sect, hdr, data) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot insert unmapped section", NULL);
      break;

    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown mode", NULL);
    }

  sect = elfsh_get_section_by_name(file, name, NULL, NULL, NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect));
}


/**
 * Insert a section at the requested index
 * Should only be used with ondisk files 
 *
 * @param file
 * @param sect
 * @param hdr
 * @param data
 * @param index
 * @return
 */
int		elfsh_insert_section_idx(elfshobj_t	*file,
					 elfshsect_t	*sect,
					 elfsh_Shdr	hdr,
					 void		*data,
					 unsigned int		index)
{
  elfshsect_t	*s;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (file == NULL || sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot get SHT", -1);

  /* Get the last current section */
  s = elfsh_get_section_by_index(file, index - 1, NULL, NULL);
  if (s == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot get last section by index", -1);

  /* Fix the file offset for the new section */
  hdr.sh_offset = s->shdr->sh_offset + s->shdr->sh_size;
  if (s->shdr->sh_addr)
    hdr.sh_addr = s->shdr->sh_addr + s->shdr->sh_size;

  /* Inject the new section and the associated header */
  if (elfsh_insert_shdr(file, hdr, index, sect->name, 1) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot insert shdr", -1);

  /* Add the section with absolute shifting */
  if (elfsh_add_section(file, sect, index, data, ELFSH_SHIFTING_COMPLETE) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot add section", -1);

  /* Inject the symbol */
  if (elfsh_insert_sectsym(file, sect) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot insert section symbol", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->index));
}


