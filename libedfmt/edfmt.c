/*
** edfmt.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 25 2006 15:41:35 mxatone
**
*/

#include "libedfmt.h"

edfmtmanage_t debug_format[] = 
  {
    { ELFSH_SECTION_NAME_DEBUG, ELFSH_SECTION_DEBUG, DWARF1_FORMAT },
    { ELFSH_SECTION_NAME_DW2_INFO, ELFSH_SECTION_DW2_INFO, DWARF2_FORMAT },
    { NULL, 0, UNKNOWN_FORMAT }
  };

/* Load a dwarf2 section information from the file */
elfshsect_t    	*edfmt_get_sect(elfshobj_t *file, u_int hash, char *hash_name)
{
  elfshsect_t	*sect;
  int		strindex;
  int		index;
  int		nbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fill the table */
  if (file->secthash[hash] == NULL)
    {
      sect = elfsh_get_section_by_name(file, hash_name,
					&index, &strindex, &nbr);
      if (NULL == sect)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get a debug format section by name", NULL);

      file->secthash[hash] = sect;

      if (file->secthash[hash]->data == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load debug format section", NULL);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     file->secthash[hash]);
}

/* Main point of the debug format library */
int		edfmt_format(elfshobj_t *file)
{
  u_int		format;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Wrong file object", -1);

  if (edfmt_format_find(file, &format) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Can't find debug format", -1);

  switch(format)
    {
    case DWARF1_FORMAT:
      printf("DWARF1 FORMAT\n");
      break;
    case DWARF2_FORMAT:
      printf("DWARF2 FORMAT\n");
      edfmt_dwarf2_parse(file);
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown debug format", -1);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

elfshsect_t   	*edfmt_format_find(elfshobj_t *file, u_int *format)
{
  u_int		i;
  elfshsect_t 	*sect = NULL;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (format != NULL)
    *format = 0;

  /* Each format */
  for (i = 0; debug_format[i].sect_name != NULL; i++)
    {
      sect = edfmt_get_sect(file, debug_format[i].sect_hash, 
			    debug_format[i].sect_name);

      if (sect != NULL)
	{
	  if (format != NULL)
	    *format = debug_format[i].format;
	  break;
	}
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sect);
}
