/*
** edfmt.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 25 2006 15:41:35 mxatone
**
*/

#include "libedfmt.h"

#define EDFMT_SET_FUNCS(_name) \
_name##_parse, _name##_transform, _name##_clean

/* This structure list all debug format that we support and with which function 
   For the moment we support stabs & dwarf2, we check if one of this debug format is present
   with checking each one sections exist.
 */
edfmtmanage_t debug_format[] = 
  {
    { ELFSH_SECTION_NAME_STAB    , ELFSH_SECTION_STAB    , EDFMT_SET_FUNCS(edfmt_stabs) },
    { ELFSH_SECTION_NAME_DW2_INFO, ELFSH_SECTION_DW2_INFO, EDFMT_SET_FUNCS(edfmt_dwarf2) },
    { NULL                       , 0                     , NULL, NULL, NULL }
  };

/* Retrieve a specific section if this section is available */
elfshsect_t    		*edfmt_get_sect(elfshobj_t *file, u_int hash, char *hash_name, 
					u_int strhash)
{
  elfshsect_t		*sect;
  int			strindex;
  int			index;
  int			nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Section pointer already set ? */
  if (file->secthash[hash] == NULL)
    {
      sect = elfsh_get_section_by_name(file, hash_name,
					&index, &strindex, &nbr);
      if (NULL == sect)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get a debug format section by name", NULL);

      file->secthash[hash] = sect;

      /* Data pointer is an assert condition */
      if (file->secthash[hash]->data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load debug format section", NULL);

      if (strhash > 0)
	{
	  sect = elfsh_get_section_by_index(file, strindex, NULL, NULL);
	  if (sect)
	    file->secthash[strhash] = sect;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     file->secthash[hash]);
}

/* Main point of the debug format library 
   This function manage this steps for every debugging formats:
    - Parse the format and create / init an interface for the next step.
    - Transform the debugging format representation in the uniform debugging format.
    - Clean elements allocated by the first step.
 */
int			edfmt_format(elfshobj_t *file)
{
  u_int			i;
  elfshsect_t 		*sect = NULL;
  u_int			count = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Wrong file object", -1);

  /* We call every elements of the structure */
  for (i = 0; debug_format[i].sect_name != NULL; i++)
    {
      /* Check if the debugging section exist */
      sect = edfmt_get_sect(file, debug_format[i].sect_hash, 
			    debug_format[i].sect_name, 0);

      if (sect != NULL && debug_format[i].func != NULL)
	{
	  /* Parse */
	  if (debug_format[i].func)
	    debug_format[i].func(file);

	  /* Transform */
	  if (debug_format[i].trans)
	    debug_format[i].trans(file);

	  /* Clean */
	  if (debug_format[i].clean)
	    debug_format[i].clean(file);
	  count++;
	}
    }

  /* No debugging format sections found */
  if (count == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Can't find at leat on debug format", -1);

  /*printf("======= ADDED TYPES ================\n");
  edfmt_uni_print(file);
  printf("====================================\n");*/

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
