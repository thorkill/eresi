/*
** sort.c for elfsh
**
** Improved by spacewalker to use quicksort
** instead of bubble sort.
** 
** Started on  Wed May 21 15:27:15 2003 mayhem
** Last update Wed May 21 15:31:57 2003 mayhem
*/
#include "libelfsh.h"


/* functions called by qsort() */
int		      sizesort_compare(const Elf32_Sym * p, const Elf32_Sym * q)
{
  if (p->st_size < q->st_size)
    return -1;
  if (p->st_size > q->st_size)
    return 1;
  return 0;
}

int		      addrsort_compare(const Elf32_Sym * p,const Elf32_Sym * q)
{
  if (p->st_value < q->st_value)
    return -1;
  if (p->st_value > q->st_value)
    return 1;
  return 0;
}

/* Sort the symtab given in parameter */
int			elfsh_sort_symtab(Elf32_Sym *symtab, int size, int type)
{

  /* Sanity checks */
  if (symtab == NULL || !size)
    ELFSH_SETERROR("[libelfsh:sort_symtab] Invalid NULL parameter\n", -1);

  switch (type) 
    {
      /* Sort by size */
    case (ELFSH_SORT_BY_SIZE):
      qsort(symtab, size, sizeof(Elf32_Sym), (void *) sizesort_compare);
      break;
      /* Sort by address */
    case (ELFSH_SORT_BY_ADDR):
      qsort(symtab, size, sizeof(Elf32_Sym), (void *)addrsort_compare);
      break;
    default:
      /* Unknown sort mode */
      ELFSH_SETERROR("[libelfsh:sort_symtab] Unknown sorting mode\n",
		     -1);
    }
  return (0);
}



/* Synchronize sorted instance of tables */
int			elfsh_sync_sorted_symtab(elfshsect_t *sect)
{
  u_int			nbr;

  if (sect == NULL || sect->shdr == NULL)
    ELFSH_SETERROR("[libelfsh:sync_sorted_symtab] Invalid NULL parameter\n", 
		   -1);
  if (sect->shdr->sh_type != SHT_SYMTAB && sect->shdr->sh_type != SHT_DYNSYM)
    ELFSH_SETERROR("[libelfsh:sync_sorted_symtab] Param is not a symtab\n", 
		   -1);

  if (sect->altdata != NULL)
    free(sect->altdata);
  nbr = sect->shdr->sh_size / sizeof(Elf32_Sym);
  sect->altdata = elfsh_copy_symtab(sect->data, nbr);
  elfsh_sort_symtab(sect->altdata, nbr, ELFSH_SORT_BY_ADDR);
  
  if (sect->terdata != NULL)
    free(sect->terdata);
  sect->terdata = elfsh_copy_symtab(sect->data, nbr);
  elfsh_sort_symtab(sect->terdata, nbr, ELFSH_SORT_BY_SIZE);

  return (0);
}
