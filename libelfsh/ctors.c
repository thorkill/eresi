/*
** ctors.c for libelfsh
**
** Started on  Tue Feb 26 22:11:12 2002 mayhem
** Last update Wed Apr 16 19:05:38 2003 mayhem
*/
#include "libelfsh.h"



/* Read the constructor array in .ctors */
u_long		*elfsh_get_ctors(elfshobj_t *file, int *num)
{
  elfshsect_t	*new;
  
  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_ctors] Invalid NULL parameter\n", NULL);

  /* Find ctors */
  new = file->secthash[ELFSH_SECTION_CTORS];
  if (new == NULL)
    {
      new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_CTORS, 
				      NULL, NULL, NULL);
      if (NULL == new)
	return (NULL);
    }
  
  /* Read ctors */
  if (NULL == new->data)
    {
      new->data = elfsh_load_section(file, new->shdr);
      if (NULL == new->data)
	return (NULL);
      file->secthash[ELFSH_SECTION_CTORS] = new;
    }

  /* Return data */
  if (num != NULL)
    *num = new->shdr->sh_size / sizeof(u_long);
  return (new->data);
}



/* Modify a CTORS entry using its index */
int		elfsh_set_ctors_entry_by_index(elfshobj_t	*file, 
					       int		index, 
					       u_long		addr)
{
  u_long	*ctors;
  int		nbr;

  ctors = elfsh_get_ctors(file, &nbr);
  if (NULL == ctors)
    return (-1);
  if (index >= nbr)
    ELFSH_SETERROR("[libelfsh:set_ctors_entry_by_index] CTORS index too big\n",
		   -1);
  ctors[index] = addr;
  return (0);
}



/* Modify a CTORS entry using its name */
int		elfsh_set_ctors_entry_by_name(elfshobj_t	*file, 
					      char		*name,
					      u_long		new_addr)
{
  u_long	*ctors;

  ctors = elfsh_get_ctors_entry_by_name(file, name);
  if (NULL == ctors)
    return (-1);
  *ctors = new_addr;
  return (0);
}



/* Return a entry giving its parent and its index */
u_long		*elfsh_get_ctors_entry_by_index(u_long *ctors, u_int index)
{
  if (NULL == ctors)
    ELFSH_SETERROR("[libelfsh:get_ctors_entry_by_index] Invalid NULL parameter\n",
		   NULL);
  return (ctors + index);
}



/* Return an entry giving the matching symbol name */
u_long		*elfsh_get_ctors_entry_by_name(elfshobj_t *file, char *name)
{
  Elf32_Sym	*sym;
  u_long	*ctors;
  u_int		nbr;
  u_int		idx;

  /* Sanity checks */
  if (file == NULL || name == NULL)
    ELFSH_SETERROR("[libelfsh:get_ctors_entry_by_name] Invalid NULL parameter\n", 
		   NULL);
  ctors = elfsh_get_ctors(file, &nbr);
  sym = elfsh_get_metasym_by_name(file, name);
  if (sym == NULL || ctors == NULL)
    return (NULL);

  /* Find the entry */
  for (idx = 0; idx < nbr; idx++)
    if (ctors[idx] == sym->st_value)
      return (ctors + idx);
  
  ELFSH_SETERROR("[libelfsh:get_ctors_entry_by_name] CTORS entry not found\n", 
		 NULL);
}


/* Return a entry giving its parent and its index : used as INTERNAL hash handler */
int		elfsh_set_ctors_entry(u_long *ctors, u_long vaddr)
{
  if (NULL == ctors)
    ELFSH_SETERROR("[libelfsh:set_ctors_entry] Invalid NULL parameter\n", -1);
  *ctors = vaddr;
  return (0);
}


/* Return a entry value giving its parent and its index : used as INTERNAL hash handler */
int		elfsh_get_ctors_entry(u_long *ctors)
{
  if (NULL == ctors)
    ELFSH_SETERROR("[libelfsh:get_ctors_entry] Invalid NULL parameter\n", -1);
  return (*ctors);
}


