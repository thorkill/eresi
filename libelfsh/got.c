/*
** got.c for elfsh
**
** Started on  Sun Jun 24 21:30:41 2001 mayhem
** Last update Thu May 15 04:39:15 2003 mayhem
*/
#include "libelfsh.h"





/* Read the global offset table in section .got */
u_long		*elfsh_get_got(elfshobj_t *file, int *num)
{
  elfshsect_t	*new;

  /* Sanity checks */
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_got] Invalid NULL parameter\n", NULL);
  else if (file->sectlist == NULL && elfsh_read_obj(file) < 0)
    return (NULL);

  /* Read GOT */
  new = file->secthash[ELFSH_SECTION_GOT];
  if (new == NULL)
    new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_GOT, NULL, NULL, NULL);
  if (new == NULL)
    return (NULL);

  /* Load GOT data */
  if (new->data == NULL)
    {
      new->data = elfsh_load_section(file, new->shdr);
      if (new->data == NULL)
	return (NULL);
      file->secthash[ELFSH_SECTION_GOT] = new;
    }

  if (num != NULL)
    *num = new->shdr->sh_size / sizeof(u_long);
  return (new->data);
}






/* Modify a GOT entry */
int		elfsh_set_got_entry_by_index(elfshobj_t *file, int i, u_long n)
{
  int		nbr;
  u_long	*got;

  got = elfsh_get_got(file, &nbr);
  if (NULL == got)
    return (-1);
  if (i >= nbr)
    ELFSH_SETERROR("[libelfsh:set_got_entry_by_index] GOT index too big\n", -1);
  got[i] = n;
  return (0);
}




/* Modify the got entry for the dynamic symbol 'n' */
int		elfsh_set_got_entry_by_name(elfshobj_t *f, char *n, u_long a)
{
  u_long	*got;

  if (f == NULL || n == NULL)
    ELFSH_SETERROR("[libelfsh:set_got_entry_by_name] Invalid NULL parameter\n",
		   -1);
  got = elfsh_get_got_entry_by_name(f, n);
  if (got == NULL)
    return (-1);
  *got = a;
  return (0);
}



/* Return a entry giving its parent and its index */
u_long		*elfsh_get_got_entry_by_index(u_long *got, u_int index)
{
  if (got == NULL)
    ELFSH_SETERROR("[libelfsh:get_got_entry_by_index] Invalid NULL parameter\n",
		   NULL);
  return (got + index);
}



/* Return a entry giving its parent and its index */
u_long		*elfsh_get_got_entry_by_name(elfshobj_t *file, char *name)
{
  u_int		nbr;
  u_int		index;
  u_long	*got;
  Elf32_Sym	*sym;
  u_int		sz;

  /* Sanity checks */
  if (file == NULL || name == NULL)
    ELFSH_SETERROR("[libelfsh:get_got_entry_by_name] Invalid NULL parameter\n",
		   NULL);
  got = elfsh_get_got(file, &nbr);
  sym = elfsh_get_dynsymbol_by_name(file, name);
  if (got == NULL || sym == NULL)
    return (NULL);

  /* Get PLT info */
  if (!elfsh_is_pltentry(file, sym))
    ELFSH_SETERROR("[libelfsh:get_got_entry_by_name] Symbol is not PLT entry\n",
		   NULL);
  sz = elfsh_get_pltentsz(file);
  
  /* Find the entry */
  for (index = 0; index < nbr; index++)
    if (INTERVAL(sym->st_value, got[index], sym->st_value + sz))
      return (got + index);
    
  /* Not found */
  ELFSH_SETERROR("[libelfsh:get_got_entry_by_name] GOT entry not found\n", 
		 NULL);
}


/* Return a entry giving its parent and its index : used as INTERNAL hash handler */
int		elfsh_set_got_entry(u_long *got, u_long vaddr)
{
  if (NULL == got)
    ELFSH_SETERROR("[libelfsh:set_got_entry] Invalid NULL parameter\n", -1);
  *got = vaddr;
  return (0);
}

/* Return a entry value giving its parent and its index : used as INTERNAL hash handler */
int		elfsh_get_got_entry(u_long *got)
{
  if (NULL == got)
    ELFSH_SETERROR("[libelfsh:get_got_entry] Invalid NULL parameter", -1);
  return (*got);
}




