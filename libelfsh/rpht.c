/*
 * rpht.c for elfsh
 *
 * Specific functions for Runtime Program Header Table
 *
 * Started on  Sat Oct 01 14:37:33 2005  mayhem
 */ 
#include "libelfsh.h"


/* Return a ptr on the program header table */
void    *elfsh_get_rpht(elfshobj_t *file, int *num)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->rpht == NULL && elfsh_create_rpht(file) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Runtime PHT does not exist", NULL);
  if (num != NULL)
    *num = file->rhdr.rphtnbr;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
                     (file->rpht));
}

/* Create and insert the Runtime PHT */
int             elfsh_create_rpht(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Create and insert the new unmapped section containing it */
  if (!file->rpht)
    {
      XALLOC(file->rpht, elfsh_get_pagesize(file), -1);
      file->rhdr.rphtnbr = 0;
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Insert a PHT entry in the RPHT (e2dbg safe) */
elfsh_Phdr	*elfsh_insert_runtime_phdr(elfshobj_t *file, elfsh_Phdr *h)
{
  elfsh_Phdr	*phdr;
  /*
  elfshsect_t	*rpht;
  elfshsect_t	*cur;
  */

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Some preliminary checks */
  phdr = elfsh_get_rpht(file, NULL);
  if (!phdr)
    elfsh_create_rpht(file);

  /*
    if (!phdr && elfsh_create_rpht(file))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
    "Unable to create Runtime PHT", NULL);
    
    rpht = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RPHT, 0, 0, 0);
    if (!rpht)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
    "Unable to get Runtime PHT", NULL);

  for (cur = rpht; cur != NULL; cur = cur->next)
    if (cur->shdr->sh_offset)
      cur->shdr->sh_offset += file->hdr->e_phentsize;

  if (file->hdr->e_shoff >= rpht->shdr->sh_offset)
    file->hdr->e_shoff += file->hdr->e_phentsize;
  */

  /* Everything OK */
  file->rpht[file->rhdr.rphtnbr++] = *h;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (file->rpht + file->rhdr.rphtnbr - 1));
}


