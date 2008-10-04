/**
 * @file strtab.c
 * @ingroup libelfsh
 * strtab.c for libelfsh
 * 
 * Started on  Sun Mar 10 03:24:23 2002 jfv
 * Last update Sun May 25 17:40:19 2003 jfv
 *
 * $Id: strtab.c,v 1.10 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libelfsh.h"

/**
 * Rebuild symbol string table if unexistant 
 * @param file
 * @return
 */
elfshsect_t	*elfsh_rebuild_strtab(elfshobj_t *file)
{
  elfshsect_t	*strtab;
  elfsh_Shdr	hdr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Create the table if it does not exist */
  if (file->secthash[ELFSH_SECTION_STRTAB] == NULL)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__,strtab, sizeof (elfshsect_t), NULL);
      hdr = elfsh_create_shdr(0, SHT_STRTAB, 0, 0, 0, 0, 0, 0, 0, 0);
      strtab->name = strdup(ELFSH_SECTION_NAME_STRTAB); 
      strtab->parent = file;
      elfsh_insert_unmapped_section(file, strtab, hdr, NULL);
      file->secthash[ELFSH_SECTION_STRTAB] = strtab;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file->secthash[ELFSH_SECTION_STRTAB]));
}

/**
 * Retreive strtab 
 * @param file
 * @param index
 * @return
 */
elfshsect_t	*elfsh_get_strtab(elfshobj_t *file, int index)
{
  elfshsect_t	*s;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  s = file->secthash[ELFSH_SECTION_STRTAB];
  if (s)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, s);
  
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
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to load STRTAB", NULL);
	} 
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
    }

  /* Section is not present */
  s = elfsh_rebuild_strtab(file);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
}

/**
 * Add a symbol name in .strtab 
 * @param file
 * @param name
 * @return
 */
int		elfsh_insert_in_strtab(elfshobj_t *file, char *name)
{
  elfshsect_t	*sect;
  unsigned int		len;
  unsigned int		index;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  sect = elfsh_get_strtab(file, -1);
  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get STRTAB", -1);

  len = strlen(name);
  if (!len)
    for (index = 0; index < sect->shdr->sh_size; index++)
      if (*(char *) sect->data + index == 0x00)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (index));

  ret = elfsh_append_data_to_section(sect, name, len + 1);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed to append data to strtab", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret); 
}


/**
 * Add a symbol name in .dynstr 
 * @param file
 * @param name
 * @return
 */
int		elfsh_insert_in_dynstr(elfshobj_t *file, char *name)
{
  elfshsect_t	*sect;
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DYNSTR, NULL, NULL, NULL);
  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find DYNSTR by name", -1);
  ret = elfsh_append_data_to_section(sect, name, strlen(name) + 1);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed to append data to dynstr", -1);
  else
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * Add a section name in section string table 
 * @param file
 * @param name
 * @return
 */
int		elfsh_insert_in_shstrtab(elfshobj_t *file, char *name)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (name == NULL || file == NULL || 
      file->secthash[ELFSH_SECTION_SHSTRTAB] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);  

  ret = elfsh_append_data_to_section(file->secthash[ELFSH_SECTION_SHSTRTAB], 
				     name, strlen(name) + 1);

  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed to append data to shstrtab", -1);
  else
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * Add a section name in runtime section string table 
 * @param file
 * @param name
 * @return
 */
int		elfsh_insert_in_rshstrtab(elfshobj_t *file, char *name)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (name == NULL || file == NULL || 
      file->secthash[ELFSH_SECTION_RSHSTRTAB] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);  

  ret = elfsh_append_data_to_section(file->secthash[ELFSH_SECTION_RSHSTRTAB],
				     name, strlen(name) + 1);

  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed to append data to rshstrtab", -1);
  else
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

