/*
** interp.c for libelfsh
** 
** Started on  Sun Jun 10 20:29:18 2001 mayhem
** Last update Sat Mar 22 15:21:56 2003 mayhem
*/
#include "libelfsh.h"



/* modify the niterpreter string : the new name len must be <= to the old one */
int             elfsh_write_interp(elfshobj_t *file, char *interp)
{
  char          *actual;
  u_int         size1;

  actual = elfsh_get_interp(file);
  if (!actual)
    ELFSH_SETERROR("[libelfsh] Cant find .interp section \n", -1);
  size1 = strlen(interp);
  if (size1 >= file->secthash[ELFSH_SECTION_INTERP]->shdr->sh_size)
    ELFSH_SETERROR("[libelfsh] New INTERP path too long \n", -1);
  strcpy(file->secthash[ELFSH_SECTION_INTERP]->data, interp);
  return (0);
}




/* Find the interpreter (pht lookup method) */
char		*elfsh_get_interp(elfshobj_t *file)
{
  elfshsect_t	*new = NULL;

  if (file->secthash[ELFSH_SECTION_INTERP] == NULL)
    {
      new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_INTERP, 
				      NULL, NULL, NULL);
      if (new == NULL)
	return (NULL);
      file->secthash[ELFSH_SECTION_INTERP] = new;
    }

  if (file->secthash[ELFSH_SECTION_INTERP]->data == NULL)
    {
      new->data = elfsh_load_section(file, 
				     file->secthash[ELFSH_SECTION_INTERP]->shdr);
      if (new->data == NULL)
	return (NULL);
    }
  return ((char *) file->secthash[ELFSH_SECTION_INTERP]->data);
}



