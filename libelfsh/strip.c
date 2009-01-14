/**
 * @file strip.c
 * @ingroup libelfsh
 * Made by jfv
 * Login   <jfv@devhell.org>
 * 
 * Started on  Sat Mar 15 03:45:54 2003 jfv
 * Last update Mon Mar 17 04:40:35 2003 jfv
 *
 * $Id$
 *
 */

#include "libelfsh.h"

/**
 * Tell elfsh to strip all unmapped sections
 * @param file
 * @return
 */
int		elfsh_strip(elfshobj_t *file)
{
  elfshsect_t	*bss;
  elfshsect_t	*next;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  bss = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_BSS,
				  NULL, NULL, NULL);
  if (file == NULL || file->sectlist == NULL || bss == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  while (bss->shdr->sh_addr)
    bss = bss->next;

  while (bss)
    {
      next = bss->next;
      if ((bss->index == 0) || 
	  ((bss->index != file->hdr->e_shstrndx) &&
	   elfsh_remove_section(file, bss->name)))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to remove section", -1);
      
      bss = next;
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

