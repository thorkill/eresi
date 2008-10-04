/**
 * @file bss.c
 * @ingroup libelfsh
** bss.c for elfsh
**
** Contain various bss handling routines for injection
**
** This file contains FILE MODIFICATION only (no debugger)
**
** Started on  Sun Apr  6 08:35:07 2003 jfv
** Last update Sat Jul 30 17:34:52 2005 jfv
**
**
** $Id: bss.c,v 1.16 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"



/**
 * Clean the BSS (remove all its data from the file and fixup PHT) 
 * @param file
 * @return
 */
int			elfsh_flush_bss(elfshobj_t *file)
{
  elfshsect_t		*bss;
  elfshsect_t		*next;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  bss = elfsh_get_section_by_name(file,
				  ELFSH_SECTION_NAME_BSS,
				  NULL, NULL, NULL);
  if (!bss)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find BSS", NULL);

  if (bss->phdr->p_filesz != bss->phdr->p_memsz)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (bss->next && bss->next->shdr->sh_addr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Not latest mapped, cannot flush", -1);

  /* Fixup PHT */
  bss->phdr->p_filesz -= bss->shdr->sh_size;
  file->hdr->e_shoff -= bss->shdr->sh_size;
  for (next = bss->next; next != NULL; next = next->next)
    next->shdr->sh_offset -= bss->shdr->sh_size;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Set memsz and filesz as seemingfull vanilla values by inadvertancy
 * Multiple BSS safe 
 * @param file
 * @param pht
 * @return
 */
int			elfsh_cleanup_bss(elfshobj_t *file, elfsh_Phdr *pht)
{
  elfshsect_t		*bss;
  unsigned int			range = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  bss = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_BSS, NULL, NULL, NULL);
  if (!bss)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find BSS", -1);

  for (range = 0; range < file->hdr->e_phnum; range++)
    if (elfsh_get_segment_type(pht + range) == PT_LOAD &&
	elfsh_segment_is_writable(pht + range))
      {
	pht[range].p_memsz += bss->shdr->sh_size;
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      }
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Cannot find data PT_LOAD",  -1);
}



/**
 * Put the bss physically in the file 
 * @param file
 * @return
 */
elfshsect_t		*elfsh_fixup_bss(elfshobj_t *file)
{
  static elfshobj_t	*obj = NULL;
  static elfshsect_t	*last = NULL;
  elfshsect_t		*bss = NULL;
  unsigned int			idx = 0;
  char			fixflag;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);

  /* We add a static variable for making the code reentrant, how fancy */
  if (file == obj && last != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (last));

  fixflag = 0;
  for (bss = file->sectlist; idx < file->hdr->e_shnum; idx++, bss = bss->next)
    if (elfsh_get_section_type(bss->shdr) == SHT_NOBITS)
      {
	if (bss->shdr->sh_size == 0)
	  {
#if	__DEBUG_BSS__
	    printf("[DEBUG_BSS] A zero sized bss has been found\n");
#endif
	    if (bss->next && bss->next->shdr->sh_addr != bss->shdr->sh_addr)
	      {
		bss->shdr->sh_size = bss->next->shdr->sh_addr - bss->shdr->sh_addr;
		fixflag = 1;
#if	__DEBUG_BSS__
		printf("[DEBUG_BSS] Fixing with size = %u \n", bss->shdr->sh_size);
#endif
	      }
	    else
	      {
#if	__DEBUG_BSS__
		puts("[DEBUG_BSS] Not fixing, continue");
#endif
		continue;
	      }
	  }
#if	__DEBUG_BSS__
	else
	  printf("[DEBUG_BSS] BSSzone found with size = %u \n", bss->shdr->sh_size);
#endif
	obj = file;
	last = bss;
	elfsh_fixup_bss_real(file, bss, fixflag);
      }
#if	__DEBUG_BSS__
  printf("[DEBUG_BSS] RETURNED %s as last bss \n", (last != 0x0) ? last->name: "null"); 
  fflush(stdout);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (last));
}



/**
 * The real function that fixup the bss
 * fixflag indicate if the section was nullsized and fixed
 * This file is not DBG safe
 *
 * @param file
 * @param bss
 * @param fixflag
 * @return
 */
int		elfsh_fixup_bss_real(elfshobj_t *file,
				     elfshsect_t *bss,
				     char fixflag)
{
  elfshsect_t	*next;
  elfshsect_t	*actual;
  unsigned int		size;
  eresi_Off	size2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fixup file offset of BSS section and makes it matches the previous
     section file offset added to the previous size. Necessary on
     Linux/MIPS GNU ld version 2.14.90.0.8 20040114, maybe others.
     If we dont do that we have a phdr size problem.
     This has to happen _BEFORE_ the shift in elfsh_add_bsszone.
  */
  if (bss->prev != NULL &&
      bss->prev->shdr->sh_offset + bss->prev->shdr->sh_size != bss->shdr->sh_offset)
    {
      size = bss->shdr->sh_offset - (bss->prev->shdr->sh_offset + bss->prev->shdr->sh_size);
      for (next = bss; next != NULL; next = next->next)
	next->shdr->sh_offset -= size;
    }

  /* Make sure the virtual address difference and the file offset difference is identical
     from the section before bss, and bss */
  if (bss->prev != NULL && bss->shdr->sh_offset - bss->prev->shdr->sh_offset != 
      bss->shdr->sh_addr - bss->prev->shdr->sh_addr)
    {
      unsigned int diff = 
	(bss->shdr->sh_addr - bss->prev->shdr->sh_addr) - 
	(bss->shdr->sh_offset - bss->prev->shdr->sh_offset);
	
      for (next = bss; next != NULL; next = next->next)
	next->shdr->sh_offset += diff;
    }


  /* Fixup file offset for section after bss in case they overlap.
     It happens on Solaris 8.0 with ld solaris-ELF 4.0
     This has to happen _BEFORE_ the shift in elfsh_add_bsszone.
  */
  if (bss->next != NULL &&
      bss->next->shdr->sh_offset <= bss->shdr->sh_offset + bss->shdr->sh_size &&
      elfsh_get_section_type(bss->next->shdr) != SHT_NOBITS && !fixflag)
    {
#if	__DEBUG_BSS__
      printf("[DEBUG_BSS] Fixing up overlapping sections\n");
#endif
      size = bss->shdr->sh_offset - bss->next->shdr->sh_offset;
      file->hdr->e_shoff += size;
      for (next = bss->next; next != NULL; next = next->next)
	next->shdr->sh_offset += size;
    }

  /* Avoid unaligned accesses on ALPHA */
  if (elfsh_get_archtype(bss->parent) == ELFSH_ARCH_ALPHA64)
    size2 = sizeof(eresi_Addr) - (bss->shdr->sh_size % sizeof(eresi_Addr));
  else
    size2 = 0;

  /* Find BSS segment */
  if (bss->phdr == NULL)
    {
      actual = bss->prev;
      while (actual->phdr == NULL)
	actual = actual->prev;
      bss->phdr = actual->phdr;
    }      

  /* Allocate BSS data */
  if (bss->data == NULL)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__,bss->data, bss->shdr->sh_size + size2, -1);
      bss->phdr->p_filesz += bss->shdr->sh_size + size2;
      bss->phdr->p_memsz  += size2;

#if __DEBUG_BSS__
      printf("Allocated BSS data of " UFMT " bytes with %u bytes of padding \n", 
	     bss->shdr->sh_size, size2);
#endif

    }

  /* Extend physical bss and the segment for .bss if PHT exists */
  else
    {
      XREALLOC(__FILE__, __FUNCTION__, __LINE__,bss->data, bss->data, bss->shdr->sh_size + size2, -1);
      bss->phdr->p_filesz += bss->shdr->sh_size + size2;
      bss->phdr->p_memsz  += size2;
      
#if __DEBUG_BSS__
      printf("[DEBUG_BSS] PT_LOAD BSS FILESZ now = " UFMT " [bss reallocated] \n", 
	     bss->shdr->sh_size + size2);
#endif
      
    }

  /* Update the SHT file offset if necessary */
  if (bss->parent->hdr->e_shoff > bss->shdr->sh_offset)
    bss->parent->hdr->e_shoff += bss->shdr->sh_size + size2;
  
  /* Update the vaddr and file offset for all next sections */
  for (actual = bss->next; actual != NULL; actual = actual->next)
    actual->shdr->sh_offset += bss->shdr->sh_size + size2;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Find the BSS section for this module 
 *
 * @param file
 * @param name
 * @return
 */
elfshsect_t	*elfsh_find_bss(elfshobj_t *file, char *name)
{
  elfshsect_t	*cur;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Find zone */
  snprintf(buff, sizeof(buff), "%s.bss", name);
  cur = elfsh_get_section_by_name(file, buff, 0, 0, 0);
  if (cur == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot find module BSS",  NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (cur));
}

/**
 * Find BSS size in a relocatable object 
 *
 * @param host
 * @param rel
 * @param bssname
 * @return
 */
int		elfsh_find_bsslen(elfshobj_t	*host,
				  elfshobj_t	*rel,
				  char		*bssname)
{
  elfshsect_t	*host_bss;
  elfshsect_t	*host_symtab;
  elfsh_Sym	*symtab;
  elfsh_Sym	enew;
  int		size;
  unsigned int		index;
  char		*name;
  char		buff[BUFSIZ];
  eresi_Addr	bss_size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (host == NULL || rel == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  /* Init some stuff */
  elfsh_get_symtab(host, NULL);
  host_symtab = host->secthash[ELFSH_SECTION_SYMTAB];
  symtab = elfsh_get_symtab(rel, &size);
  if (symtab == NULL || host_symtab == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to find symbol tables", -1);
  
  snprintf(buff, sizeof(buff), "%s%s", rel->name, bssname);
  host_bss = elfsh_get_section_by_name(host, buff, 0, 0, 0);
  if (host_bss == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Host's bss not found",  -1);

  if (host_bss == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to find host BSS", -1);

  bss_size = host_bss->shdr->sh_addr;

  /* Parse symtab and find all COMMON symbols */
  for (index = 0; index < size; index++)
    if (elfsh_get_symbol_link(symtab + index) == SHN_COMMON)
      {

	/* Align symbol size as needed */
	if (symtab[index].st_value)
	  while (bss_size % symtab[index].st_value)
	    bss_size++;


	/* Create and inject symbol in ET_EXEC */
	enew = elfsh_create_symbol(bss_size,
				  symtab[index].st_size,
				  STT_OBJECT, STB_LOCAL,
				  0, host_bss->index);

	name = elfsh_get_symbol_name(rel, symtab + index);
	elfsh_insert_symbol(host_symtab, &enew, name);
	bss_size += symtab[index].st_size;
      }

  bss_size -= host_bss->shdr->sh_addr;

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Found BSS zone lenght [%08u] for module [%s] \n",
	 bss_size, rel->name);
#endif

  elfsh_sync_sorted_symtab(host_symtab);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (bss_size));
}



/** THIS SEEMS TO BE BUGGY ON SOLARIS -- TRYING TO FIX IT --- SORRY FOR INCONVENIENCE **/

/**
 * Insert the BSS section in the destination file
 *
 * @param file
 * @param rel
 * @param bssname
 * @return
 */
elfshsect_t	*elfsh_insert_bss(elfshobj_t *file, elfshobj_t *rel, char *bssname)
{
  int		pagesize;
  elfshsect_t	*newbss;
  int		bsslen;
  char		buf[BUFSIZ];
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  pagesize = elfsh_get_pagesize(file);
  snprintf(buf, sizeof(buf), "%s%s", rel->name, bssname);
  newbss = elfsh_insert_section(file, buf, NULL, ELFSH_DATA_INJECTION, pagesize, 0);
  if (!newbss)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to insert bss section", NULL);
  
  // The size of the BSS is found in the host directly, so this call must be after the previous
  bsslen = elfsh_find_bsslen(file, rel, bssname);
  if (bsslen == -1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to find bss size", NULL);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newbss);
}




/**
 * Map a new BSS in the current file or process as an additional section 
 *
 * @param file
 * @param rel
 * @return
 */
elfshsect_t*	elfsh_insert_runtime_bss(elfshobj_t *file, elfshobj_t *rel)
{
  elfshsect_t*	newbss;
  elfshsect_t*	current;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  newbss = NULL;
  current = rel->sectlist;
  for (current = rel->sectlist; current; current = current->next)
    {
      if (elfsh_get_section_type(current->shdr) != SHT_NOBITS)
	continue;
      newbss = elfsh_insert_bss(file, rel, current->name);
      if (!newbss)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to insert runtime bss", NULL);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newbss);
    }
  
  // There was simply no bss in the relocatable file, create it with standard name
  if (!newbss)
    {
      newbss = elfsh_insert_bss(file, rel, ELFSH_SECTION_NAME_BSS);
      if (!newbss)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to insert runtime bss", NULL);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newbss);
}


/**
 * Map a new BSS in the current file or process as an additional section 
 * @param file
 * @param rel
 * @return
 */
/*
elfshsect_t	*elfsh_insert_runtime_bss(elfshobj_t *file, elfshobj_t *rel)
{
  elfshsect_t	*newbss;
  int		bsslen;
  char		buf[BUFSIZ];
  elfshsect_t	*current;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  newbss = NULL;
  current = rel->sectlist;
  for (current = rel->sectlist; current; current = current->next)
    {
      if (elfsh_get_section_type(current->shdr) != SHT_NOBITS)
	continue;
      
      snprintf(buf, sizeof(buf), "%s%s", rel->name, current->name);
      newbss = elfsh_insert_section(file, buf, NULL, ELFSH_DATA_INJECTION,
      elfsh_get_pagesize(file), 0);
      if (!newbss)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
      "Unable to insert runtime bss", NULL);
      
      bsslen = elfsh_find_bsslen(file, rel, current->name);
      if (bsslen == -1)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
      "Unable to find bss size", NULL);
    }
  
  //There was simply no bss in the relocatable file, its not an error
  if (!newbss)
    newbss = (elfshsect_t *) 0xdeadbeef;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newbss);
}
*/
