/*
** save.c for elfsh
** 
** Started on  Tue Mar  4 01:14:01 2003 mayhem
** Last update Thu Aug 21 03:36:22 2003 mayhem
*/
#include "libelfsh.h"


/* Unmap and free all ressources for this file */
int		elfsh_save_obj(elfshobj_t *file, char *name)
{
  int		fd;
  char		written_sht;
  elfshsect_t	*actual;

  /* Sanity checks */
  if (file == NULL || file->sht == NULL || file->sectlist == NULL ||
      (file->pht == NULL && elfsh_get_objtype(file->hdr) != ET_REL))
    ELFSH_SETERROR("[libelfsh:save_obj] Invalid elfshobj_t parameter\n", -1);

  /* Copy the object before saving (so that we dont strip the working file) */
  file = elfsh_copy_obj(file);
  if (file == NULL)
    return (-1);

  /* Open the output file */
  XOPEN(fd, name, O_CREAT | O_RDWR | O_SYNC, 0700, -1);

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

  /* Write the ELF header and the program header table */
  XWRITE(fd, file->hdr, file->hdr->e_ehsize, -1);
  if (file->pht != NULL)
    XWRITE(fd, file->pht, file->hdr->e_phentsize * file->hdr->e_phnum, -1);

  /* Nullify the first section size, so that we dont write it in the corefile */
  if (file->sectlist)
    file->sectlist->shdr->sh_size = 0;

  /* Write each sections in the destination file at their respective offset */
  for (written_sht = 0, actual = file->sectlist; actual; actual = actual->next)
    {

      /* If SHT is not written yet and current section file offset is bigger */
      if (!written_sht && !file->shtrm && 
	  actual->shdr->sh_offset > file->hdr->e_shoff)
	{

#if __DEBUG_MAP__
	  printf("[DEBUG_MAP] Writing SHT \t\t\t\t (file offset %u, size %u)\n",
		 file->hdr->e_shoff, 
		 file->hdr->e_shentsize * file->hdr->e_shnum);
#endif

	  XSEEK(fd, file->hdr->e_shoff, SEEK_SET, -1);
	  XWRITE(fd, file->sht, file->hdr->e_shnum * file->hdr->e_shentsize, -1);
	  written_sht = 1;
	}
      
      /* We do the check to avoid null sized section like .sbss to fool us */
      if (actual->data != NULL && 
	  actual->shdr->sh_size && 
	  !(actual->next && 
	    actual->next->shdr->sh_offset == actual->shdr->sh_offset))
	{
	  
#if __DEBUG_MAP__
	  printf("[DEBUG_MAP] Writing identified section %15s (hdr: %-15s "
		 "vaddr = %08x, idx %02u, foff %08u, size %08u, data = %08X)\n",	 
		 actual->name, elfsh_get_section_name(file, actual), 
		 elfsh_get_section_addr(actual->shdr), actual->index, 
		 elfsh_get_section_foffset(actual->shdr),
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
	       "vaddr = %08x, idx %02u, foff %08u, size %08u, data = %08X)\n", 
	       actual->name, elfsh_get_section_name(file, actual), 
	       elfsh_get_section_addr(actual->shdr), actual->index, 
	       elfsh_get_section_foffset(actual->shdr),
	       elfsh_get_section_size(actual->shdr),
	       (u_int) actual->data);
#endif

    }


  /* If SHT has not been written yet and  current section foffset is bigger */
  if (!written_sht && !file->shtrm)
    {

#if __DEBUG_MAP__
      printf("[DEBUG_MAP] Writing SHT [file offset %u, size %u]\n\n",
	     file->hdr->e_shoff, file->hdr->e_shentsize * file->hdr->e_shnum);
#endif

      XSEEK(fd, file->hdr->e_shoff, SEEK_SET, -1);
      XWRITE(fd, file->sht, file->hdr->e_shnum * file->hdr->e_shentsize, -1);
    }

#if __DEBUG_MAP__
  elfsh_print_sectlist(file);
#endif

  /* elfsh_unload_obj(file); : XXX uncomment when file is copied over saving */
  XCLOSE(fd, -1);
  return (0);
}









