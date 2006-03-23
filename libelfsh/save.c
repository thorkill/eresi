/*
** save.c for elfsh
** 
** Started on  Tue Mar  4 01:14:01 2003 mayhem
** Last update Thu Mar 23 23:21:08 2006 thorkill
*/
#include "libelfsh.h"



/* Find a string in a random buffer of size n */
static char*		elfsh_strstr(char *buffer, char *neddle, int n)
{
  int			idx;
  char			*ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (idx = 0; idx < n; idx++)
    {
      ret = strstr(buffer + idx, neddle);
      if (ret)
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Cannot find injected names", NULL);
}


/* Find the number of removed shdr before a given index */
static int		elfsh_find_previous_rmnbr(elfshobj_t *file, u_int idx)
{
  elfshsect_t		*sect;
  int			index;
  int			res;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  sect = file->sectlist;
  for (res = index = 0; index < idx; index++, sect = sect->next)
    if (sect->flags & ELFSH_SECTION_REMOVED)
      res++;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (res));
}



/* Save SHT taking care of removed shdr if the file was cleaned up */
static int		elfsh_save_sht(elfshobj_t *file, int fd)
{
  elfsh_Shdr		*newsht;
  elfshsect_t		*sect;
  int			index;
  int			wrindex;
  uint32_t		newsize;
  uint32_t		delsize;
  char			*first;
  char			*off;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_MAP__
  printf("[DEBUG_MAP] Writing SHT \t\t\t\t (file offset %u, size %u)\n",
	 (u_int) file->hdr->e_shoff, 
	 (u_int) (file->hdr->e_shentsize * file->hdr->e_shnum));
#endif
  
  /* Allocate */
  newsize = (file->hdr->e_shnum - file->nbrm) * file->hdr->e_shentsize;
  newsht = alloca(newsize);
  memset(newsht, 0x00, newsize);
  sect = file->sectlist;
  first = NULL;

  /* Dump new SHT */
  for (wrindex = index = 0; index < file->hdr->e_shnum; index++, sect = sect->next)
    if (!(sect->flags & ELFSH_SECTION_REMOVED))
      {
	memcpy(newsht + wrindex, file->sht + index, file->hdr->e_shentsize);
	newsht[wrindex].sh_link -= elfsh_find_previous_rmnbr(file, newsht[wrindex].sh_link);
#if __DEBUG_MAP__
	printf("[DEBUG_SAVE] Comparing current end from %s (%u vs %u) ! \n", 
	       sect->name, sect->curend, sect->shdr->sh_size);
#endif

	/* Avoid incorrect data dumping on partially filled sections */
	if (sect->curend)
	  {
#if __DEBUG_MAP__
	    printf("[DEBUG_SAVE] Changing %s's size for current end ! \n", sect->name);
#endif
	    newsht[wrindex].sh_size = sect->curend;
	  }
	wrindex++;
      }
    else if (wrindex == index)
      first = sect->name;

  /* Remove remaining section names in .shstrtab */
  sect = file->secthash[ELFSH_SECTION_SHSTRTAB];
  if (sect && first)
    {
      off = elfsh_strstr(sect->data, first, sect->shdr->sh_size);
      if (off)
	{
	  delsize = sect->shdr->sh_size - ((char *) off - (char *) sect->data);
	  memset(off, 0x00, delsize);
	  sect->shdr->sh_size -= delsize;
	  XSEEK(fd, elfsh_get_section_foffset(sect->shdr), SEEK_SET, -1);
	  XWRITE(fd, sect->data, sect->shdr->sh_size, -1);
	}
    }

  /* Write SHT */
  elfsh_endianize_sht((elfsh_Shdr *) newsht, 
		      file->hdr->e_ident[EI_DATA], 
		      file->hdr->e_shnum - file->nbrm);
  XSEEK(fd, file->hdr->e_shoff, SEEK_SET, 0);
  XWRITE(fd, newsht, newsize, 0);

#if __DEBUG_MAP__
  printf("[DEBUG_SAVE] Saved SHT -ok- \n");
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}


/* Unmap and free all ressources for this file */
int		elfsh_save_obj(elfshobj_t *file, char *name)
{
  int		fd;
  char		written_sht;
  elfshsect_t	*actual;
  int		index;
  elfsh_Ehdr	header;
  elfsh_Phdr	*pht;
  

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || file->sht == NULL || file->sectlist == NULL ||
      (file->pht == NULL && elfsh_get_objtype(file->hdr) != ET_REL))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid elfshobj_t parameter",  -1);

  /* Force the final relocations to be done (old_* symbols) */
  for (index = 0; index < file->nbrel; index++)
    {

#if __DEBUG_MAP__
      printf("[DEBUG_MAP] Now relocating %s in second stage \n", file->listrel[index]->name);
#endif      

      if (elfsh_relocate_object(file, file->listrel[index], ELFSH_RELOC_STAGE2) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Final stage relocation failed", -1);
    }

  /* Copy the object before saving (so that we dont strip the working file) */
  file = elfsh_copy_obj(file);
  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to copy object", -1);

  /* Open the output file */
#ifdef __BEOS__
  XOPEN(fd, name, O_CREAT | O_RDWR , 0700, -1);
#else
  XOPEN(fd, name, O_CREAT | O_RDWR | O_SYNC, 0700, -1);
#endif

  /* First reset the endianess */
  if (!file->shtrb && elfsh_dynamic_file(file))
    {
      if (elfsh_endianize_got(file->secthash[ELFSH_SECTION_GOT]) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to endianize got", -1);
      if (elfsh_endianize_symtab(file->secthash[ELFSH_SECTION_DYNSYM]) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to endianize dynsym", -1);
      if (elfsh_endianize_dynamic(file->secthash[ELFSH_SECTION_DYNAMIC]) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to endianize dynamic", -1);
    }
  if (elfsh_endianize_symtab(file->secthash[ELFSH_SECTION_SYMTAB]) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to endianize symtab", -1);

  /* Endianize multiple relocation sections */
  for (index = 0; 
       NULL != (actual = elfsh_get_reloc(file, index, NULL)); 
       index++)
    elfsh_endianize_relocs(actual);

  /* Strip the SHT if requested ... */
  if (file->shtrm)
    {
      file->hdr->e_shoff = 0;
      file->hdr->e_shstrndx = 0;
      file->hdr->e_shnum = 0;
    }

  /* .. or rewrite SHT if requested */
  else if (!file->hdr->e_shoff)
    {
      actual = elfsh_get_tail_section(file);
      file->hdr->e_shoff = actual->shdr->sh_offset + actual->shdr->sh_size;
    }

  /* Strip the file if requested */
  if (file->strip)
    elfsh_strip(file);

  /* Write the ELF header making sure removed shdr are not written */
  memcpy(&header, file->hdr, file->hdr->e_ehsize);
  header.e_shnum    -= file->nbrm;
  header.e_shstrndx -= elfsh_find_previous_rmnbr(file, header.e_shstrndx);
  elfsh_endianize_elfhdr(&header, file->hdr->e_ident[EI_DATA]);
  XWRITE(fd, &header, file->hdr->e_ehsize, -1);

  /* Write the PHT */
  if (file->pht != NULL)
    {
      pht = alloca(file->hdr->e_phnum * file->hdr->e_phentsize);
      memcpy(pht, file->pht, file->hdr->e_phnum * file->hdr->e_phentsize);
      elfsh_cleanup_bss(file, pht);
      elfsh_endianize_pht(pht, file->hdr->e_ident[EI_DATA], file->hdr->e_phnum);
      XSEEK(fd, file->hdr->e_phoff, SEEK_SET, -1);
      XWRITE(fd, pht, file->hdr->e_phentsize * file->hdr->e_phnum, -1);
    }
  
  /* Nullify the first section size, so that we dont write it in the corefile */
#if __DEBUG_MAP__
  printf("[DEBUG_SAVE] Rebuild SHT flag is set: %d\n",file->shtrb);
#endif

  if ((file->sectlist) && !file->shtrb)
    file->sectlist->shdr->sh_size = 0;

  /* Write each sections in the destination file at their respective offset */
  for (written_sht = 0, actual = file->sectlist; actual; actual = actual->next)
    {

      /* If SHT is not written yet and current section file offset is bigger */
      if (!written_sht && !file->shtrm && actual->shdr->sh_offset > file->hdr->e_shoff)
	written_sht = elfsh_save_sht(file, fd);
      
      /* We do the check to avoid null sized section like .sbss to fool us */
      if (actual->data != NULL && 
	  actual->shdr->sh_size && 
	  !(actual->next && 
	    actual->next->shdr->sh_offset == actual->shdr->sh_offset))
	{
	  
#if __DEBUG_MAP__
	  printf("[DEBUG_MAP] Writing identified section %15s (hdr: %-15s "
		 "vaddr = " AFMT ", idx %02u, foff %08u, size %08u, data = %08X)\n",	 
		 actual->name, elfsh_get_section_name(file, actual), 
		 (elfsh_Addr) elfsh_get_section_addr(actual->shdr), actual->index, 
		 (u_int) elfsh_get_section_foffset(actual->shdr),
		 elfsh_get_section_size(actual->shdr),
		 (u_int) actual->data);
#endif	  

	  /* Write the section */
	  XSEEK(fd, elfsh_get_section_foffset(actual->shdr), SEEK_SET, -1);
	  XWRITE(fd, actual->data, actual->shdr->sh_size, -1);
	 }

#if __DEBUG_MAP__      
      else
	printf("[DEBUG_MAP] PASSING identified section %15s (hdr: %-15s "
	       "vaddr = " AFMT " , idx %02u, foff %08u, size %08u, data = %08X)\n", 
	       actual->name, elfsh_get_section_name(file, actual), 
	       (elfsh_Addr) elfsh_get_section_addr(actual->shdr), actual->index, 
	       (u_int) elfsh_get_section_foffset(actual->shdr),
	       elfsh_get_section_size(actual->shdr),
	       (u_int) actual->data);
#endif

    }


  /* If SHT has not been written yet and  current section foffset is bigger */
  if (!written_sht && !file->shtrm)
    elfsh_save_sht(file, fd);

#if __DEBUG_MAP__
  /* elfsh_print_sectlist(file); */
#endif

  elfsh_unload_obj(file);
  XCLOSE(fd, -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}









