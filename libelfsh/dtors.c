/*
** dtors.c for libelfsh
**
** Started on  Tue Feb 26 22:12:38 2002 mayhem
**
*/
#include "libelfsh.h"



/* Read the destructor array in .dtors */
elfsh_Addr	*elfsh_get_dtors(elfshobj_t *file, int *num)
{
  elfshsect_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Load dtors */
  new = file->secthash[ELFSH_SECTION_DTORS];
  if (new == NULL)
    {
      new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DTORS, 
				      NULL, NULL, NULL);
      if (NULL == new)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get DTORS by name", NULL);
    }
  
  /* Read dtors */
  if (NULL == new->data)
    {
      new->data = elfsh_load_section(file, new->shdr);
      if (NULL == new->data)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load DTORS", NULL);
      file->secthash[ELFSH_SECTION_DTORS] = new;
    }

  /* Return data */
  if (num != NULL)
    *num = new->shdr->sh_size / sizeof(elfsh_Addr);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_raw(new)));
}




/* Shift DTORS on ET_DYN */
int		elfsh_shift_dtors(elfshobj_t *file, u_int size)
{
  elfshsect_t	*dtors;
  u_int		nbr;
  u_int		idx;
  elfsh_Addr	*addr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  dtors = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DTORS, 
				  NULL, NULL, &nbr);
  if (!dtors)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Cannot retreive DTORS in ET_DYN", -1);
  nbr = nbr / sizeof(elfsh_Addr);
  for (idx = 0; idx < nbr; idx++)
    {
      addr = elfsh_get_dtors_entry_by_index(dtors->data, idx);
      if (*addr && *addr != ELFSH_END_DTORS)
	*addr += size;
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}





/* Modify a DTORS entry */
int		elfsh_set_dtors_entry_by_index(elfshobj_t	*file, 
					       int		index, 
					       elfsh_Addr      	addr)
{
  int		nbr;
  elfsh_Addr	*dtors;
  u_int		size;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  dtors = elfsh_get_dtors(file, &nbr);
  if (NULL == dtors)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get DTORS entry", -1);

  size = file->secthash[ELFSH_SECTION_DTORS]->shdr->sh_size / sizeof(elfsh_Addr);
  if (index >= size)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "DTORS index too big", -1);

  dtors[index] = addr;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Modify a DTORS entry using its name */
int		elfsh_set_dtors_entry_by_name(elfshobj_t	*file, 
					      char		*name,
					      elfsh_Addr       	new_addr)
{
  elfsh_Addr	*dtors;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  dtors = elfsh_get_dtors_entry_by_name(file, name);
  if (NULL == dtors)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to set DTORS entry by name", -1);
  *dtors = new_addr;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Get DTORS entry by name */
elfsh_Addr     	*elfsh_get_dtors_entry_by_name(elfshobj_t *file, char *name)
{
  elfsh_Sym	*sym;
  elfsh_Addr	*dtors;
  u_int		nbr;
  u_int		idx;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  dtors = elfsh_get_dtors(file, &nbr);
  sym = elfsh_get_metasym_by_name(file, name);
  if (sym == NULL || dtors == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DTORS entry by name", NULL);

  /* Find the entry */
  for (idx = 0; idx < nbr; idx++)
    if (dtors[idx] == sym->st_value)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (dtors + idx));
  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "DTORS entry not found", NULL);
}



/* Return a entry giving its parent and its index : used as INTERNAL hash handler */
elfsh_Addr     	*elfsh_get_dtors_entry_by_index(elfsh_Addr *dtors, elfsh_Addr index)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (dtors == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (dtors + index));
}


/* Return a entry giving its parent and its index : used as INTERNAL hash handler */
int		elfsh_set_dtors_entry(elfsh_Addr *dtors, elfsh_Addr vaddr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == dtors)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  *dtors = vaddr;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Return a entry value giving its parent and its index : used as INTERNAL hash handler */
elfsh_Addr     	elfsh_get_dtors_entry(elfsh_Addr *dtors)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == dtors)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (*dtors));
}
