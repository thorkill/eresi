/*
** dtors.c for libelfsh
**
** Started on  Tue Feb 26 22:12:38 2002 mayhem
** Last update Wed Apr 16 19:04:39 2003 mayhem
*/
#include "libelfsh.h"



/* Read the destructor array in .dtors */
u_long		*elfsh_get_dtors(elfshobj_t *file, int *num)
{
  elfshsect_t	*new;

  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_dtors] Invalid NULL parameter\n", NULL);

  /* Load dtors */
  new = file->secthash[ELFSH_SECTION_DTORS];
  if (new == NULL)
    {
      new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DTORS, 
				      NULL, NULL, NULL);
      if (NULL == new)
	return (NULL);
    }
  
  /* Read dtors */
  if (NULL == new->data)
    {
      new->data = elfsh_load_section(file, new->shdr);
      if (NULL == new->data)
	return (NULL);
      file->secthash[ELFSH_SECTION_DTORS] = new;
    }

  /* Return data */
  if (num != NULL)
    *num = new->shdr->sh_size / sizeof(u_long);
  return (new->data);
}



/* Modify a DTORS entry */
int		elfsh_set_dtors_entry_by_index(elfshobj_t	*file, 
					       int		index, 
					       u_long		addr)
{
  int		nbr;
  u_long	*dtors;
  u_int		size;

  dtors = elfsh_get_dtors(file, &nbr);
  if (NULL == dtors)
    return (-1);
  size = file->secthash[ELFSH_SECTION_DTORS]->shdr->sh_size / sizeof(u_long);
  if (index >= size)
    ELFSH_SETERROR("[libelfsh:set_dtors_entry_by_index] DTORS index too big\n",
		   -1);
  dtors[index] = addr;
  return (0);
}





/* Modify a DTORS entry using its name */
int		elfsh_set_dtors_entry_by_name(elfshobj_t	*file, 
					      char		*name,
					      u_long		new_addr)
{
  u_long	*dtors;

  dtors = elfsh_get_dtors_entry_by_name(file, name);
  if (NULL == dtors)
    return (-1);
  *dtors = new_addr;
  return (0);
}


/* Get DTORS entry by name */
u_long		*elfsh_get_dtors_entry_by_name(elfshobj_t *file, char *name)
{
  Elf32_Sym	*sym;
  u_long	*dtors;
  u_int		nbr;
  u_int		idx;

  /* Sanity checks */
  if (file == NULL || name == NULL)
    ELFSH_SETERROR("[libelfsh:get_dtors_entry_by_name] Invalid NULL parameter\n", 
		   NULL);
  dtors = elfsh_get_dtors(file, &nbr);
  sym = elfsh_get_metasym_by_name(file, name);
  if (sym == NULL || dtors == NULL)
    return (NULL);

  /* Find the entry */
  for (idx = 0; idx < nbr; idx++)
    if (dtors[idx] == sym->st_value)
      return (dtors + idx);
  
  ELFSH_SETERROR("[libelfsh:get_dtors_entry_by_name] DTORS entry not found\n", 
		 NULL);
}



/* Return a entry giving its parent and its index : used as INTERNAL hash handler */
u_long		*elfsh_get_dtors_entry_by_index(u_long *dtors, u_int index)
{
  if (dtors == NULL)
    ELFSH_SETERROR("[libelfsh:get_dtors_entry_by_index] Invalid NULL parameter\n",
		   NULL);
  return (dtors + index);
}


/* Return a entry giving its parent and its index : used as INTERNAL hash handler */
int		elfsh_set_dtors_entry(u_long *dtors, u_long vaddr)
{
  if (NULL == dtors)
    ELFSH_SETERROR("[libelfsh:set_dtors_entry] Invalid NULL parameter\n", -1);
  *dtors = vaddr;
  return (0);
}


/* Return a entry value giving its parent and its index : used as INTERNAL hash handler */
int		elfsh_get_dtors_entry(u_long *dtors)
{
  if (NULL == dtors)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  return (*dtors);
}
