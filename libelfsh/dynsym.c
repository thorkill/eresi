/*
** Dynsym.c for libelfsh
** 
** Started on  Mon Feb 26 04:13:29 2001 mayhem
** Last update Sun Jun  1 06:50:39 2003 mayhem
*/
#include "libelfsh.h"





/* Return the dynsymbol name giving its index in the dynamic symbol string table */
char		*elfsh_get_dynsymbol_name(elfshobj_t *file, Elf32_Sym *s)
{
  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL && 
      NULL == elfsh_get_dynsymtab(file, NULL))
    return (NULL);
  return ((char *) file->secthash[ELFSH_SECTION_DYNSTR]->data + s->st_name);
}



/* Return the used offset in .dynstr or -1 if failed */
int		elfsh_set_dynsymbol_name(elfshobj_t *file, Elf32_Sym *s, char *name)
{
  char		*str;
  u_int		len;
  u_int		new_len;

  if (file == NULL || s == NULL || name == NULL)
    ELFSH_SETERROR("[libelfsh:set_dynsymbol_name] Invalid NULL parameter", -1);

  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL && 
      elfsh_get_dynsymtab(file, NULL) == NULL)
    ELFSH_SETERROR("[libelfsh:get_dynsymbol_name] Cannot retreive symbol table",
		   -1);

  /* Else use the symbol string table */
  if (file->secthash[ELFSH_SECTION_DYNSTR] == NULL || 
      file->secthash[ELFSH_SECTION_DYNSTR]->data == NULL)
    return (NULL);

  /* Change the name */
  str = file->secthash[ELFSH_SECTION_DYNSTR]->data;
  str += s->st_name;
  len = strlen(str);
  new_len = strlen(name);

  /* Do not allocate new place if possible */
  if (len >= new_len)
    strcpy(str, name);
  /* Append the name to .strtab */
  else
    s->st_name = elfsh_insert_in_dynstr(file, name);

  return (s->st_name);
}






/* Return a ptr on the dynamic symbol table num is filled with the entries total number */
void		*elfsh_get_dynsymtab(elfshobj_t *file, int *num)
{
  elfshsect_t	*new;
  int		strindex;
  int		nbr;

  if (file == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", NULL);


  /* Load the .dynsym */
  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL)
    {
      new = elfsh_get_section_by_type(file, SHT_DYNSYM, 0, NULL, &strindex, &nbr);
      if (new == NULL)
	return (NULL);
      new->data = elfsh_load_section(file, new->shdr);
      if (new->data == NULL) 
	return (NULL);
      file->secthash[ELFSH_SECTION_DYNSYM] = new;

      /* Now retreive the dynamic symbol string table .dynstr */
      new = elfsh_get_section_by_index(file, strindex, NULL, &nbr);
      if (new == NULL)
	return (NULL);
      new->data = elfsh_load_section(file, new->shdr);
      if (new->data == NULL)
	return (NULL);
      file->secthash[ELFSH_SECTION_DYNSTR] = new;

      /* Sort the table */
      elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_DYNSYM]);
    }

  if (num != NULL)
    *num = 
      file->secthash[ELFSH_SECTION_DYNSYM]->shdr->sh_size / sizeof(Elf32_Sym);
  return (file->secthash[ELFSH_SECTION_DYNSYM]->data);
}



/* 
** Return the dynamic symbol name giving its value, 
** Fill 'offset' with the difference between sym->st_value and 'value'
*/
char		*elfsh_reverse_dynsymbol(elfshobj_t	*file,
					 u_int		value, 
					 int		*offset)
{
  elfshsect_t	*sect;
  Elf32_Sym	*sorted;
  int		num;
  int		index;
  char		*str;

  /* Sanity checks */
  if (!value || value == 0xFFFFFFFF)
    return (NULL);  
  if (file == NULL)
    ELFSH_SETERROR("[libelfsh:reverse_dynsymbol] Invalid NULL parameter\n", 
		   NULL);
  *offset = 0;

  /* If there is no symtab, resolve using SHT */
  if (elfsh_get_dynsymtab(file, &num) == NULL)
    {
      sect = elfsh_get_parent_section(file, value, offset);
      if (sect == NULL)
	ELFSH_SETERROR("[libelfsh:reverse_dynsymbol] No parent section\n", 
		       NULL);
      *offset = (int) sect->shdr->sh_addr - value;
      return (elfsh_get_section_name(file, sect));
    }

  /* Else use the sorted-by-address symbol table to match what we want */
  if (file->secthash[ELFSH_SECTION_DYNSYM]->altdata == NULL)
    elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_DYNSYM]);
  sorted = file->secthash[ELFSH_SECTION_DYNSYM]->altdata;

  for (index = 0; index < num; index++)
    if (sorted[index].st_value <= value && DUMPABLE(sorted + index) &&
	(index + 1 >= num || sorted[index + 1].st_value > value))
      {
	*offset = (int) value - sorted[index].st_value;
	str = elfsh_get_dynsymbol_name(file, sorted + index);
	if (!*str)
	  str = NULL;
	return (str);
      }

  /* Not found */
  ELFSH_SETERROR("[libelfsh:reverse_dynsymbol] No valid symbol interval\n", 
		 NULL);
}






/* Return the symbol entry giving its name */
Elf32_Sym	*elfsh_get_dynsymbol_by_name(elfshobj_t *file, char *name)
{
  Elf32_Sym	*ret;
  int		index;
  int		size = 0;
  char		*actual;

  if (file == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter", NULL);
  if (NULL == elfsh_get_dynsymtab(file, &size))
    return (NULL);
  if (NULL == name)
    return (NULL);

  ret = file->secthash[ELFSH_SECTION_DYNSYM]->data;
  for (index = 0; index < size; index++)
    {
      actual = elfsh_get_dynsymbol_name(file, ret + index);
      if (actual && !strcmp(actual, name))
	return (ret + index);
    }
  
  ELFSH_SETERROR("[libelfsh] Symbol not found", NULL);
}


























