/**
 * @file stabs.c
 * @ingroup libelfsh
 * stabs.c for libelfsh
 * 
 * Started on  Mon Feb 26 04:14:06 2001 jfv
 * 
 *
 * $Id: stab.c,v 1.9 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libelfsh.h"

/**
 * @brief Return the debugging symbol name giving its index in 
 * the stab string table.
 * @param file
 * @param s
 * @return
 */
char	*elfsh_get_stab_name(elfshobj_t *file, elfshstabent_t *s)
{
  char	*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL file parameter",  NULL);

  if (NULL == file->secthash[ELFSH_SECTION_STABSTR] &&
      NULL == elfsh_get_stab(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive stabs section",  NULL);

  str = (char *) elfsh_get_raw(file->secthash[ELFSH_SECTION_STABSTR]) + s->strindex;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (str));
}

/**
 * @brief Load the stab information from the file.
 * @param file
 * @param num
 * @return
 */
void		*elfsh_get_stab(elfshobj_t *file, int *num)
{
  elfshsect_t	*sect;
  int		strindex;
  int		index;
  int		nbr;
  void		*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  /* Fill the stab table */
  if (file->secthash[ELFSH_SECTION_STAB] == NULL)
    {

      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_STAB,
					&index, &strindex, &nbr);
      if (NULL == sect)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get STABS by name", NULL);

      file->secthash[ELFSH_SECTION_STAB] = sect;
      sect->data = elfsh_load_section(file, sect->shdr);

      if (sect->data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load STABS", NULL);

      /* Fill the stab string table */
      sect = elfsh_get_section_by_index(file, strindex, NULL, NULL);

      if (sect == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get STABS string table", NULL);

      file->secthash[ELFSH_SECTION_STABSTR] = sect;
      sect->data = elfsh_load_section(file, sect->shdr);

      if (sect->data == NULL)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load STABS string table", NULL);
    }

  if (num != NULL)
    {
      nbr = file->secthash[ELFSH_SECTION_STAB]->shdr->sh_size;
      *num = nbr / sizeof(elfshstabent_t);
    }

  ret = elfsh_get_raw(file->secthash[ELFSH_SECTION_STAB]);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * @brief Return the symbol type name giving its index in the symtype array 
 * @param s
 * @return
 */
unsigned int	elfsh_get_stab_type(elfshstabent_t *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->type));
}

/**
 * Not used yet 
 * @param s
 * @return
 */
eresi_Addr	elfsh_get_stab_offset(elfshstabent_t *s)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (((eresi_Addr) (s->value))));
}
