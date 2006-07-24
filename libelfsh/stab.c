/*
** stabs.c for libelfsh
**
** Started on  Mon Feb 26 04:14:06 2001 mayhem
**
*/
#include "libelfsh.h"




/* Return the debugging symbol name giving its index in the stab string table */
char	*elfsh_get_stab_name(elfshobj_t *file, elfshstabent_t *s)
{
  char	*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL file parameter",  NULL);

  if (NULL == file->secthash[ELFSH_SECTION_STABSTR] &&
      NULL == elfsh_get_stab(file, NULL))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive stabs section",  NULL);

  str = (char *) elfsh_get_raw(file->secthash[ELFSH_SECTION_STABSTR]) + s->strindex;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (str));
}





/* Load the stab information from the file */
void		*elfsh_get_stab(elfshobj_t *file, int *num)
{
  elfshsect_t	*sect;
  int		strindex;
  int		index;
  int		nbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  /* Fill the stab table */
  if (file->secthash[ELFSH_SECTION_STAB] == NULL)
    {

      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_STAB,
					&index, &strindex, &nbr);
      if (NULL == sect)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get STABS by name", NULL);

      file->secthash[ELFSH_SECTION_STAB] = sect;
      file->secthash[ELFSH_SECTION_STAB]->data =
	elfsh_load_section(file, sect->shdr);
      if (file->secthash[ELFSH_SECTION_STAB]->data == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load STABS", NULL);

      /* Fill the stab string table */
      sect = elfsh_get_section_by_index(file, strindex, NULL, NULL);
      if (sect == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get STABS string table", NULL);

      sect->data = elfsh_load_section(file, sect->shdr);
      if (sect->data == NULL)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load STABS string table", NULL);

      file->secthash[ELFSH_SECTION_STABSTR] = sect;
    }

  if (num != NULL)
    *num = file->secthash[ELFSH_SECTION_STAB]->shdr->sh_size /
      sizeof(elfshstabent_t);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_raw(file->secthash[ELFSH_SECTION_STAB])));
}




/* Return the symbol type name giving its index in the symtype array */
u_int	elfsh_get_stab_type(elfshstabent_t *s)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->type));
}



/* Not used yet */
elfsh_Addr	elfsh_get_stab_offset(elfshstabent_t *s)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (((elfsh_Addr) (s->value))));
}
