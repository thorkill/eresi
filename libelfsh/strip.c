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
  elfshsect_t	*bss;
  elfshsect_t	*next;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  bss = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_BSS,
				  NULL, NULL, NULL);
  if (file == NULL || file->sectlist == NULL || bss == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  while (bss->shdr->sh_addr)
    bss = bss->next;
  while (bss)
    {
      next = bss->next;
      if (bss->index != file->hdr->e_shstrndx &&
	  elfsh_remove_section(file, bss->name))
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to remove section", -1);
      bss = next;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


