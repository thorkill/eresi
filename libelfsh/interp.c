/** 
 * @file interp.c
 * @ingroup libelfsh
** interp.c for libelfsh
** 
** Started on  Sun Jun 10 20:29:18 2001 jfv
** 
**
** $Id: interp.c,v 1.7 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"



/**
 * modify the niterpreter string : the new name len must be <= to the old one 
 * @param file
 * @param interp
 * @return
 */
int             elfsh_write_interp(elfshobj_t *file, char *interp)
{
  char          *actual;
  unsigned int         size1;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  actual = elfsh_get_interp(file);
  if (!actual)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cant find .interp section", -1);

  size1 = strlen(interp);
  if (size1 >= file->secthash[ELFSH_SECTION_INTERP]->shdr->sh_size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "New INTERP path too long", -1);

  strncpy(elfsh_get_raw(file->secthash[ELFSH_SECTION_INTERP]), interp, size1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Find the interpreter (pht lookup method) 
 * @param file
 * @return
 */
char		*elfsh_get_interp(elfshobj_t *file)
{
  elfshsect_t	*enew = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_INTERP] == NULL)
    {
      enew = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_INTERP, 
				      NULL, NULL, NULL);
      if (enew == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get .interp by name", NULL);
      file->secthash[ELFSH_SECTION_INTERP] = enew;
    }

  if (file->secthash[ELFSH_SECTION_INTERP]->data == NULL)
    {
      enew->data = elfsh_load_section(file, 
				     file->secthash[ELFSH_SECTION_INTERP]->shdr);
      if (enew->data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load .interp", NULL);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		((char *) elfsh_get_raw(file->secthash[ELFSH_SECTION_INTERP])));
}



