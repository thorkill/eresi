/**
 * @file sort.c
 * @ingroup libelfsh
** sort.c for elfsh
** 
** Improved by spacewalker to use quicksort from libc
** instead of some ugly homemade bubble sort
** 
** Started on  Wed May 21 15:27:15 2003 jfv
** 
**
** $Id: sort.c,v 1.9 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"

elfshsect_t *symstrsect = NULL;

/**
 * @brief functions called by qsort()
 * @param p
 * @param q
 * @return
 */
int		      sizesort_compare(const elfsh_Sym * p, const elfsh_Sym * q)
{
  if (p->st_size < q->st_size)
    return -1;
  if (p->st_size > q->st_size)
    return 1;
  return 0;
}

/**
 * @brief Missing
 * @param p
 * @param q
 * @return
 */
int		      addrsort_compare(const elfsh_Sym * p,const elfsh_Sym * q)
{
  if (p->st_value < q->st_value)
    return -1;
  if (p->st_value > q->st_value)
    return 1;
  return 0;
}

/**
 * @brief Sort the symtab given in parameter 
 * @param symtab
 * @param size
 * @param type
 * @return
 */
int			elfsh_sort_symtab(elfsh_Sym *symtab, int size, int type)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (symtab == NULL || !size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  switch (type) 
    {
      /* Sort by size */
    case (ELFSH_SORT_BY_SIZE):
      qsort(symtab, size, sizeof(elfsh_Sym), (void *) sizesort_compare);
      break;
      /* Sort by address */
    case (ELFSH_SORT_BY_ADDR):
      qsort(symtab, size, sizeof(elfsh_Sym), (void *) addrsort_compare);
      break;
    default:
      /* Unknown sort mode */
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown sorting mode", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Synchronize sorted instance of tables 
 * @param sect
 * @return
 */
int			elfsh_sync_sorted_symtab(elfshsect_t *sect)
{
  unsigned int			nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sect == NULL || sect->shdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);

  if (sect->shdr->sh_type != SHT_SYMTAB && sect->shdr->sh_type != SHT_DYNSYM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Param is not a symtab", -1);

  nbr = sect->curend / sizeof(elfsh_Sym);

  if (nbr == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Current section is empty", -1);

  /* Sort by address */
  if (sect->altdata != NULL)
    XFREE(__FILE__, __FUNCTION__, __LINE__,sect->altdata);

  sect->altdata = elfsh_copy_symtab(sect->data, nbr);
  elfsh_sort_symtab(sect->altdata, nbr, ELFSH_SORT_BY_ADDR);
  
  /* Sort by size */
  if (sect->terdata != NULL)
    XFREE(__FILE__, __FUNCTION__, __LINE__,sect->terdata);

  sect->terdata = elfsh_copy_symtab(sect->data, nbr);
  elfsh_sort_symtab(sect->terdata, nbr, ELFSH_SORT_BY_SIZE);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
