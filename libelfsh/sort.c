/*
** sort.c for elfsh
**
** Improved by spacewalker to use quicksort from libc
** instead of some ugly homemade bubble sort
** 
** Started on  Wed May 21 15:27:15 2003 mayhem
**
*/
#include "libelfsh.h"


/* functions called by qsort() */
int		      sizesort_compare(const elfsh_Sym * p, const elfsh_Sym * q)
{
  if (p->st_size < q->st_size)
    return -1;
  if (p->st_size > q->st_size)
    return 1;
  return 0;
}

int		      addrsort_compare(const elfsh_Sym * p,const elfsh_Sym * q)
{
  if (p->st_value < q->st_value)
    return -1;
  if (p->st_value > q->st_value)
    return 1;
  return 0;
}

/* Sort the symtab given in parameter */
int			elfsh_sort_symtab(elfsh_Sym *symtab, int size, int type)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (symtab == NULL || !size)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  switch (type) 
    {
      /* Sort by size */
    case (ELFSH_SORT_BY_SIZE):
      qsort(symtab, size, sizeof(elfsh_Sym), (void *) sizesort_compare);
      break;
      /* Sort by address */
    case (ELFSH_SORT_BY_ADDR):
      qsort(symtab, size, sizeof(elfsh_Sym), (void *)addrsort_compare);
      break;
    default:
      /* Unknown sort mode */
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown sorting mode", -1);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Synchronize sorted instance of tables */
int			elfsh_sync_sorted_symtab(elfshsect_t *sect)
{
  u_int			nbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sect == NULL || sect->shdr == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid NULL parameter", 
		      -1);
  if (sect->shdr->sh_type != SHT_SYMTAB && sect->shdr->sh_type != SHT_DYNSYM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Param is not a symtab", 
		      -1);

  if (sect->altdata != NULL)
    XFREE(sect->altdata);

  nbr = sect->curend / sizeof(elfsh_Sym);
  //nbr = sect->shdr->sh_size / sizeof(elfsh_Sym);
  sect->altdata = elfsh_copy_symtab(sect->data, nbr);
  elfsh_sort_symtab(sect->altdata, nbr, ELFSH_SORT_BY_ADDR);
  
  if (sect->terdata != NULL)
    XFREE(sect->terdata);
  sect->terdata = elfsh_copy_symtab(sect->data, nbr);
  elfsh_sort_symtab(sect->terdata, nbr, ELFSH_SORT_BY_SIZE);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
