/**
* @file libelfsh/rpht.c
 * @ingroup libelfsh
** rpht.c for elfsh
** 
** @brief Specific functions for Runtime Program Header Table
** 
** Started on  Sat Oct 01 14:37:33 2005  jfv
**
** $Id$
**
*/
#include "libelfsh.h"


/**
 * @brief Return a ptr on the program header table 
 * @param file
 * @param num
 * @return
 */
void    *elfsh_get_rpht(elfshobj_t *file, int *num)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->rpht == NULL && elfsh_create_rpht(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Runtime PHT does not exist", NULL);
  if (num != NULL)
    *num = file->rhdr.rphtnbr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
                     (file->rpht));
}

/**
 * Create and insert the Runtime PHT 
 * @param file
 * @return
 */
int             elfsh_create_rpht(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Create and insert the new unmapped section containing it */
  if (!file->rpht)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__,file->rpht, elfsh_get_pagesize(file), -1);
      file->rhdr.rphtnbr = 0;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Insert a PHT entry in the RPHT (e2dbg safe) 
 * @param file
 * @param h
 * @return
 */
elfsh_Phdr	*elfsh_insert_runtime_phdr(elfshobj_t *file, elfsh_Phdr *h)
{
  elfsh_Phdr	*phdr;
  /*
  elfshsect_t	*rpht;
  elfshsect_t	*cur;
  */

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Some preliminary checks */
  phdr = elfsh_get_rpht(file, NULL);
  if (!phdr)
    elfsh_create_rpht(file);

  /*
    if (!phdr && elfsh_create_rpht(file))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
    "Unable to create Runtime PHT", NULL);
    
    rpht = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RPHT, 0, 0, 0);
    if (!rpht)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
    "Unable to get Runtime PHT", NULL);

  for (cur = rpht; cur != NULL; cur = cur->next)
    if (cur->shdr->sh_offset)
      cur->shdr->sh_offset += file->hdr->e_phentsize;

  if (file->hdr->e_shoff >= rpht->shdr->sh_offset)
    file->hdr->e_shoff += file->hdr->e_phentsize;
  */

  /* Everything OK */
  file->rpht[file->rhdr.rphtnbr++] = *h;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (file->rpht + file->rhdr.rphtnbr - 1));
}


