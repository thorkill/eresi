/*
** edfmt.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 25 2006 15:41:35 mxatone
**
*/

#include "libedfmt.h"

edfmtmanage_t debug_format[] = 
  {
    { DWARF1_SECTION, DWARF1_FORMAT },
    { DWARF2_SECTION, DWARF2_FORMAT },
    { NULL, UNKNOWN_FORMAT }
  };

/* Main point of the debug format library */
int		edfmt_format(elfshobj_t *file)
{
  u_int		format;
  elfshsect_t	*sect;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Wrong file object", -1);

  sect = edfmt_format_find(file, &format);

  if (sect == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Can't find debug format", -1);

  switch(format)
    {
    case DWARF1_FORMAT:
      printf("DWARF1 FORMAT\n");
      break;
    case DWARF2_FORMAT:
      printf("DWARF2 FORMAT\n");
      edfmt_dwarf2_parse(file, sect);
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
      sect = elfsh_get_section_by_name(file, 
				       debug_format[i].sect_name,
				       NULL, NULL, NULL);

      if (sect != NULL)
	{
	  if (format != NULL)
	    *format = debug_format[i].format;
	  break;
	}
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sect);
}
