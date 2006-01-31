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
  elfsh_Shdr	hdr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Create the table if it does not exist */
  if (file->secthash[ELFSH_SECTION_STRTAB] == NULL)
    {
      XALLOC(strtab, sizeof (elfshsect_t), NULL);
      hdr = elfsh_create_shdr(0, SHT_STRTAB, 0, 0, 0, 0, 0, 0, 0, 0);

      /* Use elfsh_strdup from libcelfsh */
      strtab->name = elfsh_strdup(ELFSH_SECTION_NAME_STRTAB);
      strtab->parent = file;
      elfsh_insert_unmapped_section(file, strtab, hdr, NULL);
      file->secthash[ELFSH_SECTION_STRTAB] = strtab;
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->secthash[ELFSH_SECTION_STRTAB]));
}





/* Retreive strtab */
elfshsect_t	*elfsh_get_strtab(elfshobj_t *file, int index)
{
  elfshsect_t	*s;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  else if (file->secthash[ELFSH_SECTION_STRTAB] != NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->secthash[ELFSH_SECTION_STRTAB]));
  
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
      s->shdr->sh_link = file->secthash[ELFSH_SECTION_SYMTAB]->index;
      if (s->data == NULL)
	{
	  s->data = elfsh_load_section(file, s->shdr);
	  if (s->data == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to load STRTAB", NULL);
	} 
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
    }

  /* Section is not present */
  s = elfsh_rebuild_strtab(file);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
}



/* Add a symbol name in .strtab */
int		elfsh_insert_in_strtab(elfshobj_t *file, char *name)
{
  elfshsect_t	*sect;
  u_int		len;
  u_int		index;
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  sect = elfsh_get_strtab(file, -1);
  if (sect == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get STRTAB", -1);

  len = strlen(name);
  if (!len)
    for (index = 0; index < sect->shdr->sh_size; index++)
      if (*(char *) sect->data + index == 0x00)
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (index));

  ret = elfsh_append_data_to_section(sect, name, len + 1);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed to append data to strtab", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret); 
}


/* Add a symbol name in .dynstr */
int		elfsh_insert_in_dynstr(elfshobj_t *file, char *name)
{
  elfshsect_t	*sect;
  int		ret;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DYNSTR, NULL, NULL, NULL);
  if (sect == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find DYNSTR by name", -1);
  ret = elfsh_append_data_to_section(sect, name, strlen(name) + 1);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed to append data to dynstr", -1);
  else
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Add a section name in section string table */
int		elfsh_insert_in_shstrtab(elfshobj_t *file, char *name)
{
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (name == NULL || file == NULL || 
      file->secthash[ELFSH_SECTION_SHSTRTAB] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);  

  ret = elfsh_append_data_to_section(file->secthash[ELFSH_SECTION_SHSTRTAB], 
				     name, strlen(name) + 1);

  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed to append data to shstrtab", -1);
  else
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Add a section name in runtime section string table */
int		elfsh_insert_in_rshstrtab(elfshobj_t *file, char *name)
{
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (name == NULL || file == NULL || 
      file->secthash[ELFSH_SECTION_RSHSTRTAB] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);  

  ret = elfsh_append_data_to_section(file->secthash[ELFSH_SECTION_RSHSTRTAB],
				     name, strlen(name) + 1);

  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed to append data to rshstrtab", -1);
  else
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}




