/*
** Dynsym.c for libelfsh
** 
** Started on  Mon Feb 26 04:13:29 2001 mayhem
**
*/
#include "libelfsh.h"





/* Return the dynsymbol name giving its index in the dynamic symbol string table */
char		*elfsh_get_dynsymbol_name(elfshobj_t *file, elfsh_Sym *s)
{
  char	*ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !s)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL)
    if (NULL == elfsh_get_dynsymtab(file, NULL))
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unable to get DYNSYM", NULL);

  ret = (char *) elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSTR]) + s->st_name;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/* Return the used offset in .dynstr or -1 if failed */
int		elfsh_set_dynsymbol_name(elfshobj_t *file, elfsh_Sym *s, char *name)
{
  char		*str;
  u_int		len;
  u_int		new_len;
  void		*data;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || s == NULL || name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL && 
      elfsh_get_dynsymtab(file, NULL) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive symbol table", -1);

  /* Else use the symbol string table */
  data = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSTR]);
  if (file->secthash[ELFSH_SECTION_DYNSTR] == NULL || 
      data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DYNSTR", NULL);

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

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->st_name));
}






/* Return a ptr on the dynamic symbol table num is filled with the entries total number */
void		*elfsh_get_dynsymtab(elfshobj_t *file, int *num)
{
  elfshsect_t	*new;
  int		strindex;
  int		nbr;
  void		*ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Load the .dynsym */
  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL)
    {
      new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTDYNSYM, 
				      NULL, &strindex, &nbr);
      if (new == NULL)
	{
	  new = elfsh_get_section_by_type(file, SHT_DYNSYM, 0,
					  NULL, &strindex, &nbr);
	  if (new == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to get DYNSYM by type", NULL);
	}

      new->data = elfsh_load_section(file, new->shdr);
      if (new->data == NULL) 
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load DYNSYM", NULL);
      file->secthash[ELFSH_SECTION_DYNSYM] = new;
      
      /* Endianize table */
      elfsh_endianize_symtab(new); 
      
      /* Now retreive the dynamic symbol string table .dynstr */
      new = elfsh_get_section_by_index(file, strindex, NULL, &nbr);
      if (new == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get DYNSTR by index", NULL);

      new->data = elfsh_load_section(file, new->shdr);
      if (new->data == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load DYNSTR", NULL);

      file->secthash[ELFSH_SECTION_DYNSTR] = new;

      /* Fixup the dynamic symbol table */
      elfsh_fixup_dynsymtab(file->secthash[ELFSH_SECTION_DYNSYM]);

      /* Sort the table */
      elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_DYNSYM]);
      new->curend = new->shdr->sh_size;
    }

  /*
  else
    puts("Not entered in NULL dynsym cond\n");
  */  

  nbr = 
    file->secthash[ELFSH_SECTION_DYNSYM]->curend ? 
    file->secthash[ELFSH_SECTION_DYNSYM]->curend / sizeof(elfsh_Sym) : 
    file->secthash[ELFSH_SECTION_DYNSYM]->shdr->sh_size / sizeof(elfsh_Sym);
    
  if (num != NULL)
    *num = nbr;

  /*
  printf(__FUNCTION__ " has dynsym data = %08X pdata = %08X \n", 
	 file->secthash[ELFSH_SECTION_DYNSYM]->data, 
	 file->secthash[ELFSH_SECTION_DYNSYM]->pdata);
  */

  ret = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSYM]);
  
  //printf(__FUNCTION__ " has dynsym ret : %x\n", ret);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (ret));
}



/* 
** Return the dynamic symbol name giving its value, 
** Fill 'offset' with the difference between sym->st_value and 'value'
*/
char		*elfsh_reverse_dynsymbol(elfshobj_t	*file,
					 elfsh_Addr	value, 
					 elfsh_SAddr    *offset)
{
  elfshsect_t	*sect;
  elfsh_Sym	*sorted;
  int		num;
  int		index;
  char		*str;

  elfsh_Sym	*pltsym;
  elfshsect_t	*plt;
  

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (!value || value == (elfsh_Addr) -1)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);  
  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  if (offset)
    *offset = 0;

  /* If there is no symtab, resolve using SHT */
  if (elfsh_get_dynsymtab(file, &num) == NULL) 
    {
      sect = elfsh_get_parent_section(file, value, offset);
      if (sect == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No parent section", NULL);

      /* handle dynamic case */
      if (elfsh_is_debug_mode())
	value -= file->rhdr.base;

      if (offset)
	*offset = (elfsh_SAddr) (sect->shdr->sh_addr - value);

      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_section_name(file, sect)));
    }

  /* Else use the sorted-by-address symbol table to match what we want */
  if (file->secthash[ELFSH_SECTION_DYNSYM]->altdata == NULL)
    elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_DYNSYM]);
  sorted = file->secthash[ELFSH_SECTION_DYNSYM]->altdata;

  /* Restore dynsym if pointing inside PLT and we did not find it */ 
  plt = file->secthash[ELFSH_SECTION_PLT];
  sect = elfsh_get_parent_section(file, value, offset); 
  if (plt && sect && sect->name && !strcmp(sect->name, ELFSH_SECTION_NAME_PLT))
    {
      pltsym = elfsh_restore_dynsym(file, plt, *offset,
				    file->secthash[ELFSH_SECTION_DYNSYM]);
      if (pltsym)
	{
	  *offset = 0;
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
			     (elfsh_get_dynsymbol_name(file, pltsym)));
	}                                                                                                                                   
    }

  /* handle dynamic case */
  if (elfsh_is_debug_mode())
    value -= file->rhdr.base;

  /* Else look in the table */
  for (index = 0; index < num; index++)
    if (sorted[index].st_value <= value && DUMPABLE(sorted + index) &&
	(index + 1 >= num || sorted[index + 1].st_value > value))
      {
	
	if (offset)
	    *offset = (elfsh_SAddr) (value - sorted[index].st_value);
	str = elfsh_get_dynsymbol_name(file, sorted + index);
	if (!*str)
	  str = NULL;
	
	
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (str));
      }
  
  /* Not found */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "No valid symbol interval", NULL);
}






/* Return the symbol entry giving its name */
elfsh_Sym	*elfsh_get_dynsymbol_by_name(elfshobj_t *file, char *name)
{
  elfsh_Sym	*ret;
  int		index;
  int		size = 0;
  char		*actual;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  ret = elfsh_get_dynsymtab(file, &size);
  if (ret == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DYNSYM", NULL);

  for (index = 0; index < size; index++)
    {
      actual = elfsh_get_dynsymbol_name(file, ret + index);
      if (actual && !strcmp(actual, name))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret + index));
    }
  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Symbol not found", NULL);
}



/* Shift the dynamic symbol table, mostly useful on ET_DYN objects */
int		elfsh_shift_dynsym(elfshobj_t *file, elfsh_Addr limit, int inc)
{
  elfshsect_t	*actual;
  int		err;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  actual = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTDYNSYM, 
				     NULL, NULL, NULL);
  if (!actual)
    {
      actual = elfsh_get_section_by_type(file, SHT_DYNSYM, 0, NULL, NULL, NULL);
      if (actual == NULL || actual->data == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to find DYNSYM by type", -1);
    }

  err = elfsh_shift_syms(file, actual, limit, inc);
  if (err < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to shift DYNSYM", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
