/*
** strtab.c for libelfsh
** 
** Started on  Sun Mar 10 03:24:23 2002 mayhem
** Last update Sun May 25 17:40:19 2003 mayhem
*/
#include "libelfsh.h"



/* Rebuild symbol string table if unexistant */
elfshsect_t	*elfsh_rebuild_strtab(elfshobj_t *file)
{
  elfshsect_t	*strtab;
  Elf32_Shdr	hdr;

  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:rebuild_strtab] Invalid NULL parameter\n", NULL);

  /* Create the table if it does not exist */
  if (file->secthash[ELFSH_SECTION_STRTAB] == NULL)
    {
      XALLOC(strtab, sizeof (elfshsect_t), NULL);
      hdr = elfsh_create_shdr(0, SHT_STRTAB, 0, 0, 0, 0, 0, 0, 0, 0);
      strtab->name = strdup(ELFSH_SECTION_NAME_STRTAB);
      strtab->parent = file;
      elfsh_insert_unmapped_section(file, strtab, hdr, NULL);
      file->secthash[ELFSH_SECTION_STRTAB] = strtab;
    }
  return (file->secthash[ELFSH_SECTION_STRTAB]);
}





/* Retreive strtab */
elfshsect_t	*elfsh_get_strtab(elfshobj_t *file, int index)
{
  elfshsect_t	*s;

  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_strtab] Invalid NULL parameter\n",
		   NULL);
  else if (file->secthash[ELFSH_SECTION_STRTAB] != NULL)
    return (file->secthash[ELFSH_SECTION_STRTAB]);
  
  /* Read the string table */
  if (index > 0)
    s = elfsh_get_section_by_index(file, index, NULL, NULL);
  else
    s = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_STRTAB,
				  NULL, NULL, NULL);
  
  /* Section is present */
  if (s != NULL)
    {
      file->secthash[ELFSH_SECTION_STRTAB] = s;
      s->index = file->secthash[ELFSH_SECTION_SYMTAB]->shdr->sh_link;
      if (s->data == NULL)
	{
	  s->data = elfsh_load_section(file, s->shdr);
	  if (s->data == NULL)
	    return (NULL);
	} 
      return (s);
    }

  /* Section is not present */
  s = elfsh_rebuild_strtab(file);
  return (s);
}



/* Add a symbol name in .strtab */
int		elfsh_insert_in_strtab(elfshobj_t *file, char *name)
{
  elfshsect_t	*sect;
  u_int		len;
  u_int		index;

  if (file == NULL || name == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  sect = elfsh_get_strtab(file, -1);
  if (sect == NULL)
    return (-1);

  len = strlen(name);
  if (!len)
    for (index = 0; index < sect->shdr->sh_size; index++)
      if (*(char *) sect->data + index == 0x00)
	return (index);

  /* printf("[DEBUG_SOLBUG] Inserting *%s* into strtab section \n", name); */

  return (elfsh_append_data_to_section(sect, name, len + 1));
}


/* Add a symbol name in .dynstr */
int		elfsh_insert_in_dynstr(elfshobj_t *file, char *name)
{
  elfshsect_t	*sect;
  
  if (file == NULL || name == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DYNSTR, NULL, NULL, NULL);
  if (sect == NULL)
    ELFSH_SETERROR("[libelfsh] No .dynstr section\n", -1);
  return (elfsh_append_data_to_section(sect, name, strlen(name) + 1));
}


/* Add a section name in .shstrtab */
int		elfsh_insert_in_shstrtab(elfshobj_t *file, char *name)
{
  if (name == NULL || file == NULL || 
      file->secthash[ELFSH_SECTION_SHSTRTAB] == NULL)
    ELFSH_SETERROR("[libelfsh:insert_in_shstrtab] Invalid NULL parameter\n", -1);  
  return (elfsh_append_data_to_section(file->secthash[ELFSH_SECTION_SHSTRTAB], 
				       name, strlen(name) + 1));
}




