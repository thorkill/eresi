/*
** map.c for libelfsh
**
** Started on  Sat Mar  2 20:47:36 2002 mayhem
** Last update Thu Aug 21 03:34:55 2003 mayhem
*/
#include "libelfsh.h"


/* Fixup the binary, inject symbols and sort SHT */
void		elfsh_fixup(elfshobj_t *file)
{
  elfsh_get_symtab(file, NULL);
  if (file->hdr->e_type == ET_REL)
    elfsh_sort_sht(file);
}


/* Load all the part of the binary */
int		elfsh_read_obj(elfshobj_t *file)
{
  elfshsect_t	*actual;
  int		index;

  if (file->read)
    return (0);
  if (file->sht == NULL && NULL == elfsh_get_sht(file, NULL))
    return (-1);
  if (NULL == elfsh_get_pht(file, NULL) && file->hdr->e_type != ET_REL)
    return (-1);

#if __DEBUG_MAP__
  puts("[DEBUG:read_obj] Loading all known typed sections\n");
#endif

  /* Fill multiple relocation sections */
  for (index = 0; 
       NULL != (actual = elfsh_get_reloc(file, index, NULL)); 
       index++);

  /*
  ** We cannot use simply elfsh_get_anonymous_section() for all these,
  ** since the ptrs in the elfshobj_t section hash would not be filled.
  */


  elfsh_get_symtab(file, NULL);
  elfsh_get_dynsymtab(file, NULL);
  elfsh_get_stab(file, NULL);
  elfsh_get_dynamic(file, NULL);
  elfsh_get_ctors(file, NULL);
  elfsh_get_dtors(file, NULL);
  elfsh_get_got(file, NULL);
  elfsh_get_interp(file);
  elfsh_get_hashtable(file);
  elfsh_get_comments(file);
  elfsh_get_plt(file, NULL);


  /* Fill the multiple notes sections */
  for (index = 0; NULL != elfsh_get_notes(file, index); index++);

  /* Loop on the section header table and load all unknown-typed sections */
  for (actual = file->sectlist; actual; actual = actual->next)
    {

      /* Fix first section size */
      if (actual->shdr->sh_size == 0 && actual->next &&
	  actual->next->shdr->sh_offset != actual->shdr->sh_offset)
	actual->shdr->sh_size =
	  actual->next->shdr->sh_offset - actual->shdr->sh_offset;

      /* If the section data has to be loaded, load it */
      if (actual->data == NULL && actual->shdr->sh_size)
	{

	  if (actual->shdr->sh_type == SHT_NOBITS ||
	      (actual->next != NULL &&
	       actual->next->shdr->sh_offset == actual->shdr->sh_offset))
	    continue;

#if __DEBUG_MAP__
	  printf("[LIBELFSH] Loading anonymous  section %15s \n",
		 elfsh_get_section_name(file, actual));
#endif

	  elfsh_get_anonymous_section(file, actual);
	}

    }

  elfsh_fixup(file);
  return (0);
}



/* Map in memory all ressources for this file */
elfshobj_t	*elfsh_map_obj(char *name)
{
  elfshobj_t	*file;

  file = elfsh_load_obj(name);
  if (file == NULL)
    return (NULL);
  file->rights = O_RDWR;
  elfsh_read_obj(file);
  return (file);
}
