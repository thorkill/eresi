/**
 * @file ctors.c
 * ctors.c for libelfsh
 *
 * Started on  Tue Feb 26 22:11:12 2002 jfv
 *
 * $Id: ctors.c,v 1.7 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libelfsh.h"



/**
 * Read the constructor array in .ctors 
 * @param file
 * @param num
 * @return
 */
elfsh_Addr      *elfsh_get_ctors(elfshobj_t *file, int *num)
{
  elfshsect_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Sanity checks */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Find ctors */
  new = file->secthash[ELFSH_SECTION_CTORS];
  if (new == NULL)
    {
      new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_CTORS, 
				      NULL, NULL, NULL);
      if (NULL == new)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get CTORS by name", NULL);
    }
  
  /* Read ctors */
  if (NULL == new->data)
    {
      new->data = elfsh_load_section(file, new->shdr);
      if (NULL == new->data)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load CTORS", NULL);
      file->secthash[ELFSH_SECTION_CTORS] = new;
    }

  /* Return data */
  if (num != NULL)
    *num = new->shdr->sh_size / sizeof(elfsh_Addr);

  /* Return a pointer on the data. Also work in debug mode */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_raw(new)));
}




/**
 * Shift CTORS on ET_DYN 
 * @param file
 * @param size
 * @return
 */
int		elfsh_shift_ctors(elfshobj_t *file, u_int size)
{
  elfshsect_t	*ctors;
  int		nbr;
  u_int		idx;
  elfsh_Addr	*addr;
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ctors = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_CTORS, 
				  NULL, NULL, &nbr);
  if (!ctors)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Cannot retreive CTORS in ET_DYN", -1);
  nbr = nbr / sizeof(elfsh_Addr);
  for (idx = 0; idx < nbr; idx++)
    {
      addr = elfsh_get_ctors_entry_by_index(ctors->data, idx);
      if (*addr && *addr != ELFSH_END_CTORS)
	*addr += size;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}



/**
 * Modify a CTORS entry using its index 
 * @param file
 * @param index
 * @param addr
 * @return
 */
int		elfsh_set_ctors_entry_by_index(elfshobj_t	*file, 
					       int		index, 
					       elfsh_Addr     	addr)
{
  elfsh_Addr	*ctors;
  int		nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ctors = elfsh_get_ctors(file, &nbr);
  if (NULL == ctors)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get CTORS", -1);

  if (index >= nbr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "CTORS index too big", -1);

  ctors[index] = addr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Modify a CTORS entry using its name 
 * @param file
 * @param name
 * @param new_addr
 * @return
 */
int		elfsh_set_ctors_entry_by_name(elfshobj_t	*file, 
					      char		*name,
					      elfsh_Addr       	new_addr)
{
  elfsh_Addr	*ctors;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ctors = elfsh_get_ctors_entry_by_name(file, name);
  if (NULL == ctors)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get CTORS entry by name", -1);
  *ctors = new_addr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * Return a entry giving its parent and its index 
 * @param ctors
 * @param index
 * @return
 */
elfsh_Addr     	*elfsh_get_ctors_entry_by_index(elfsh_Addr *ctors, elfsh_Addr index)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == ctors)
PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid NULL parameter",
		   NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ctors + index));
}



/**
 * Return an entry giving the matching symbol name 
 * @param file
 * @param name
 * @return
 */
elfsh_Addr     	*elfsh_get_ctors_entry_by_name(elfshobj_t *file, char *name)
{
  elfsh_Sym	*sym;
  elfsh_Addr	*ctors;
  int		nbr;
  u_int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid NULL parameter", 
		   NULL);
  ctors = elfsh_get_ctors(file, &nbr);
  sym = elfsh_get_metasym_by_name(file, name);
  if (sym == NULL || ctors == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find symbol with this name", NULL);

  /* Find the entry */
  for (idx = 0; idx < nbr; idx++)
    if (ctors[idx] == sym->st_value)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ctors + idx));
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "CTORS entry not found", 
		 NULL);
}


/**
 * Return a entry giving its parent and its index : used as INTERNAL hash handler 
 * @param ctors
 * @param vaddr
 * @return
 */
int		elfsh_set_ctors_entry(elfsh_Addr *ctors, elfsh_Addr vaddr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == ctors)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid NULL parameter", -1);
  *ctors = vaddr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Return a entry value giving its parent and its index : used as INTERNAL hash handler 
 * @param ctors
 * @return
 */
elfsh_Addr     	elfsh_get_ctors_entry(elfsh_Addr *ctors)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == ctors)
PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (*ctors));
}


