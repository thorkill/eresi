/*
** stabs.c for libelfsh
**
** Started on  Mon Feb 26 04:14:06 2001 mayhem
** Last update Sat Aug 16 13:42:24 2003 jv
*/
#include "libelfsh.h"




/* Return the debugging symbol name giving its index in the stab string table */
char	*elfsh_get_stab_name(elfshobj_t *file, elfshstabent_t *s)
{
  char	*str;

  if (file == NULL)
    ELFSH_SETERROR("libelfsh: Invalid NULL file parameter", NULL);
  if (NULL == file->secthash[ELFSH_SECTION_STABSTR] &&
      NULL == elfsh_get_stab(file, NULL))
    ELFSH_SETERROR("libelfsh: Cannot retreive stabs section", NULL);

  str = (char *) file->secthash[ELFSH_SECTION_STABSTR]->data + s->strindex;
  return (str);
}





/* Load the stab information from the file */
void		*elfsh_get_stab(elfshobj_t *file, int *num)
{
  elfshsect_t	*sect;
  u_int		strindex;
  u_int		index;
  u_int		nbr;

  /*
    #if defined(sun)
    ELFSH_SETERROR("[libelfsh:get_stab] Stabs unimplemented on SUN\n", NULL);
    #endif
  */

  if (file->secthash[ELFSH_SECTION_STAB] == NULL)
    {

      /* Fill the stab table */
      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_STAB,
				       &index, &strindex, &nbr);
      if (NULL == sect)
        return (NULL);
      file->secthash[ELFSH_SECTION_STAB] = sect;
      file->secthash[ELFSH_SECTION_STAB]->data =
	elfsh_load_section(file, sect->shdr);
      if (file->secthash[ELFSH_SECTION_STAB]->data == NULL)
	return (NULL);

      /* Fill the stab string table */
      sect = elfsh_get_section_by_index(file, strindex, NULL, NULL);
      if (sect == NULL)
	return (NULL);
      sect->data = elfsh_load_section(file, sect->shdr);
      if (sect->data == NULL)
        return (NULL);
      file->secthash[ELFSH_SECTION_STABSTR] = sect;
    }

  if (num != NULL)
    *num = file->secthash[ELFSH_SECTION_STAB]->shdr->sh_size /
      sizeof(elfshstabent_t);
  return (file->secthash[ELFSH_SECTION_STAB]->data);
}




/* Return the symbol type name giving its index in the symtype array */
u_int	elfsh_get_stab_type(elfshstabent_t *s)
{
  return (s->type);
}



/* Not used yet */
void	*elfsh_get_stab_offset(elfshstabent_t *s)
{
  return ((void *) ((int) (s->value)));
}
