/*
** dynamic.c for libelfsh
** 
** Started on  Tue Feb 27 04:36:53 2001 mayhem
** Last update Sat May 24 00:25:51 2003 mayhem
*/
#include "libelfsh.h"



/* Return a ptr on the dynamic section */
Elf32_Dyn	*elfsh_get_dynamic(elfshobj_t *file, int *num)
{
  elfshsect_t	*new = NULL; /* to shut gcc up with -Wall */
  int		nbr;

  if (file->secthash[ELFSH_SECTION_DYNAMIC] == NULL)
    {
      
      new = elfsh_get_section_by_type(file, SHT_DYNAMIC, NULL, NULL, &nbr, 0);
      if (new == NULL)
	return (NULL);
    
      new->data = elfsh_load_section(file, new->shdr);
      if (new->data == NULL)
	return (NULL);

      file->secthash[ELFSH_SECTION_DYNAMIC] = new;
    }
  
  if (num != NULL)
    *num = 
      file->secthash[ELFSH_SECTION_DYNAMIC]->shdr->sh_size / sizeof(Elf32_Dyn);
  
  return (file->secthash[ELFSH_SECTION_DYNAMIC]->data);
}




/* Return the tag field of the dynamic entry */
Elf32_Sword	elfsh_get_dynentry_type(Elf32_Dyn *d)
{
  if (d == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  return (d->d_tag);
}

/* Change the tag field of the dynamic entry */
int		elfsh_set_dynentry_type(Elf32_Dyn *d, Elf32_Sword tag)
{
  if (d == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  d->d_tag = tag;
  return (0);
}

/* Return the tag field of the dynamic entry */
Elf32_Word	elfsh_get_dynentry_val(Elf32_Dyn *d)
{
  if (d == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  return (d->d_un.d_val);
}

/* Change the val field of the dynamic entry */
int		elfsh_set_dynentry_val(Elf32_Dyn *d, Elf32_Word val)
{
  if (d == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", -1);
  d->d_un.d_val = val;
  return (0);
}


/* Retreive the information giving the entry */
char		*elfsh_get_dynentry_string(elfshobj_t *file, Elf32_Dyn *ent)
{
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:get_dynentry_string] Invalid NULL parameter\n",
		   NULL);
  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL && 
      elfsh_get_dynsymtab(file, NULL) == NULL)
    return (NULL);
  if (ent->d_un.d_val > file->secthash[ELFSH_SECTION_DYNSTR]->shdr->sh_size)
    ELFSH_SETERROR("[libelfsh:get_dynentry_string] Invalid .dynstr offset\n",
		   NULL);
  return ((char *) file->secthash[ELFSH_SECTION_DYNSTR]->data + ent->d_un.d_val);
}


/* Get .dynamic entries by type */
Elf32_Dyn	*elfsh_get_dynamic_entry_by_type(elfshobj_t *file, char type)
{
  int		index;
  int		size;
  Elf32_Dyn	*table;

  table = elfsh_get_dynamic(file, &size);
  if (!table)
    return (NULL);

  for (index = 0; index < size; index++)
    if (elfsh_get_dynentry_type(table + index) == type)
      return (table + index);
  
  ELFSH_SETERROR("libelfsh: No dynamic entry with that type .", NULL);
}


/* Return a Elf32_Dyn entry by its index */
Elf32_Dyn	*elfsh_get_dynamic_entry_by_index(Elf32_Dyn *dynamic, u_int index)
{
  return (dynamic + index);
}










