/** 
 * @file interp.c
** interp.c for libelfsh
** 
** Started on  Sun Jun 10 20:29:18 2001 mayhem
** 
**
** $Id: interp.c,v 1.5 2007-06-27 11:25:12 heroine Exp $
**
*/
#include "libelfsh.h"



/**
 * modify the niterpreter string : the new name len must be <= to the old one 
 */
int             elfsh_write_interp(elfshobj_t *file, char *interp)
{
  char          *actual;
  u_int         size1;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  actual = elfsh_get_interp(file);
  if (!actual)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cant find .interp section", -1);

  size1 = strlen(interp);
  if (size1 >= file->secthash[ELFSH_SECTION_INTERP]->shdr->sh_size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "New INTERP path too long", -1);

  strcpy(elfsh_get_raw(file->secthash[ELFSH_SECTION_INTERP]), interp);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Find the interpreter (pht lookup method) 
 */
char		*elfsh_get_interp(elfshobj_t *file)
{
  elfshsect_t	*new = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_INTERP] == NULL)
    {
      new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_INTERP, 
				      NULL, NULL, NULL);
      if (new == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get .interp by name", NULL);
      file->secthash[ELFSH_SECTION_INTERP] = new;
    }

  if (file->secthash[ELFSH_SECTION_INTERP]->data == NULL)
    {
      new->data = elfsh_load_section(file, 
				     file->secthash[ELFSH_SECTION_INTERP]->shdr);
      if (new->data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load .interp", NULL);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		((char *) elfsh_get_raw(file->secthash[ELFSH_SECTION_INTERP])));
}



