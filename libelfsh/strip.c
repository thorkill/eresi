/*
** strip.c for  in 
** 
** Made by mayhem
** Login   <mayhem@devhell.org>
** 
** Started on  Sat Mar 15 03:45:54 2003 mayhem
** Last update Mon Mar 17 04:40:35 2003 mayhem
*/
#include "libelfsh.h"



/* Tell elfsh to strip all unmapped sections */
int		elfsh_strip(elfshobj_t *file)
{
  elfshsect_t	*unmapped1;
  elfshsect_t	*shstrtab;
  /* char		*name; */

  if (file == NULL || file->sectlist == NULL)
    ELFSH_SETERROR("[libelfsh:strip] Invalid NULL parameter\n", -1);

  /* XXX: strip buggy, put it implemented */
  return (0);

  /* Get elfshsect_t for .shstrtab */
  shstrtab = elfsh_get_section_by_index(file, 
					file->hdr->e_shstrndx,
					NULL,
					NULL);
  
  /* Find the first unmapped section which is not .shstrtab */
  unmapped1 = file->sectlist; 
  while (unmapped1 && unmapped1->shdr->sh_addr == NULL && 
	 unmapped1->index != file->hdr->e_shstrndx)
    unmapped1 = unmapped1->next;

  /* Swap sections headers if act is before shstrtab */
  if (unmapped1->shdr->sh_offset < shstrtab->shdr->sh_offset)
    {
      shstrtab->shdr->sh_offset = unmapped1->shdr->sh_offset;
      *unmapped1->shdr = *shstrtab->shdr;
    }
  
  /* Erase all unmapped sections */
  unmapped1 = shstrtab->next;
  for (shstrtab->next = NULL; unmapped1; unmapped1 = unmapped1->next)
    {
      free(unmapped1->data);
      file->hdr->e_shnum--;
      bzero(&file->sht[unmapped1->index], file->hdr->e_shentsize);
    }

  return (0);
}


