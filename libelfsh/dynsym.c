/**
 * @file dynsym.c
 * @ingroup libelfsh
** Dynsym.c for libelfsh
** 
** Started on  Mon Feb 26 04:13:29 2001 jfv
** 
**
** $Id: dynsym.c,v 1.21 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"


/**
 * Return the dynsymbol name giving its index in the dynamic symbol string table 
 * @param file
 * @param s
 * @return
 */
char		*elfsh_get_dynsymbol_name(elfshobj_t *file, elfsh_Sym *s)
{
  char	*ret;
  void	*rdata;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL)
    if (NULL == elfsh_get_dynsymtab(file, NULL))
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unable to get DYNSYM", NULL);
  
  rdata = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSTR]);
  ret = (char *) rdata + s->st_name;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/**
 * Return the used offset in .dynstr or -1 if failed 
 * @param file
 * @param s
 * @param name
 * @return
 */
int		elfsh_set_dynsymbol_name(elfshobj_t *file, 
					 elfsh_Sym *s, char *name)
{
  char		*str;
  u_int		len;
  u_int		new_len;
  void		*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || s == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL && 
      elfsh_get_dynsymtab(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive symbol table", -1);

  /* Else use the symbol string table */
  data = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSTR]);
  if (file->secthash[ELFSH_SECTION_DYNSTR] == NULL || 
      data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DYNSTR", NULL);

  /* Change the name */
  str = (char *) file->secthash[ELFSH_SECTION_DYNSTR]->data;
  str += s->st_name;
  len = strlen(str);
  new_len = strlen(name);

  /* Do not allocate new place if possible */
  if (len >= new_len)
    strncpy(str, name, new_len);
  /* Append the name to .strtab */
  else
    s->st_name = elfsh_insert_in_dynstr(file, name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s->st_name));
}






/**
 * Return a ptr on the dynamic symbol table num is filled with the entries total number 
 * @param file
 * @param num
 * @return
 */
void		*elfsh_get_dynsymtab(elfshobj_t *file, int *num)
{
  elfshsect_t	*newent;
  int		strindex;
  int		nbr;
  void		*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  //fprintf(stderr, "Calling get_dynsymtab ! \n");

  /* Load the .dynsym */
  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL)
    {
      newent = elfsh_get_section_by_name(file, 
					 ELFSH_SECTION_NAME_ALTDYNSYM, 
					 NULL, &strindex, &nbr);
      if (newent == NULL)
	{
	  newent = elfsh_get_section_by_type(file, SHT_DYNSYM, 0,
					     NULL, &strindex, &nbr);
	  if (newent == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to get DYNSYM by type", NULL);
	}

      newent->data = elfsh_load_section(file, newent->shdr);
      if (newent->data == NULL) 
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load DYNSYM", NULL);
      file->secthash[ELFSH_SECTION_DYNSYM] = newent;
      
      /* Endianize table */
      elfsh_endianize_symtab(newent); 
      
      /* Now retreive the dynamic symbol string table .dynstr */
      newent = elfsh_get_section_by_index(file, strindex, NULL, &nbr);
      if (newent == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get DYNSTR by index", NULL);

      newent->data = elfsh_load_section(file, newent->shdr);
      if (newent->data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load DYNSTR", NULL);

      file->secthash[ELFSH_SECTION_DYNSTR] = newent;

      /* Fixup the dynamic symbol table */
      elfsh_fixup_dynsymtab(file->secthash[ELFSH_SECTION_DYNSYM]);

      /* Sort the table */
      elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_DYNSYM]);
      newent->curend = newent->shdr->sh_size;
    }

  /*
  fprintf(stderr, "dynsym curend = %u (%u entries) shsize = %u (%u entries) \n", 
	  file->secthash[ELFSH_SECTION_DYNSYM]->curend,
	  file->secthash[ELFSH_SECTION_DYNSYM]->curend / sizeof(elfsh_Sym),
	  file->secthash[ELFSH_SECTION_DYNSYM]->shdr->sh_size,
	  file->secthash[ELFSH_SECTION_DYNSYM]->shdr->sh_size / sizeof(elfsh_Sym));
  */

  nbr = 
    file->secthash[ELFSH_SECTION_DYNSYM]->curend ? 
    file->secthash[ELFSH_SECTION_DYNSYM]->curend / sizeof(elfsh_Sym) : 
    file->secthash[ELFSH_SECTION_DYNSYM]->shdr->sh_size / sizeof(elfsh_Sym);
    
  if (num != NULL)
    *num = nbr;

  ret = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSYM]);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * Get symtab entry by vaddr 
 * @param file
 * @param vaddr
 * @param off
 * @param mode
 * @return
 */
elfsh_Sym	  *elfsh_get_dynsymbol_by_value(elfshobj_t	*file,
						eresi_Addr	vaddr,
						int	*off,
						int	mode)
{
  int		  num;
  elfsh_Sym	  *data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_dynsymtab(file, &num);
  if (data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive DYNSYM", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_get_sym_by_value(data, num, vaddr, off, mode)));
}


/** 
 * Return the dynamic symbol name giving its value, 
 * Fill 'offset' with the difference between sym->st_value and 'value'
 *
 * @param file
 * @param value
 * @param offset
 * @return
 */
char		*elfsh_reverse_dynsymbol(elfshobj_t	*file,
					 eresi_Addr	value, 
					 elfsh_SAddr    *offset)
{
  elfshsect_t	*sect;
  elfsh_Sym	*sorted;
  int		num;
  int		index;
  char		*str;
  elfsh_Sym	*pltsym;
  elfshsect_t	*plt;
  

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (!value || value == (eresi_Addr) -1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);  
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  if (offset)
    *offset = 0;

  /* If there is no symtab, resolve using SHT */
  if (elfsh_get_dynsymtab(file, &num) == NULL) 
    {
      sect = elfsh_get_parent_section(file, value, offset);
      if (sect == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No parent section", NULL);

      /* handle dynamic case */
      if (elfsh_is_debug_mode())
	value -= file->rhdr.base;

      if (offset)
	*offset = (elfsh_SAddr) (sect->shdr->sh_addr - value);

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (elfsh_get_section_name(file, sect)));
    }

  /* Else use the sorted-by-address symbol table to match what we want */
  if (file->secthash[ELFSH_SECTION_DYNSYM]->altdata == NULL)
    elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_DYNSYM]);
  sorted = (elfsh_Sym *) file->secthash[ELFSH_SECTION_DYNSYM]->altdata;

  /* Restore dynsym if pointing inside PLT and we did not find it */ 
  plt = file->secthash[ELFSH_SECTION_PLT];
  sect = elfsh_get_parent_section(file, value, offset); 
  if (plt && sect && sect->name && !strcmp(sect->name, ELFSH_SECTION_NAME_PLT))
    {
      pltsym = elfsh_restore_dynsym(file, plt, *offset,
				    file->secthash[ELFSH_SECTION_DYNSYM]);
      if (pltsym)
	{
	  if (offset)
	    *offset = 0;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
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
	
	
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (str));
      }
  
  /* Not found */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "No valid symbol interval", NULL);
}


/**
 * Return the symbol entry giving its name 
 * @param file target file
 * @param name dynamic symbol name
 * @return symbol pointer or NULL
 */
elfsh_Sym	*elfsh_get_dynsymbol_by_name(elfshobj_t *file, char *name)
{
  elfsh_Sym	*sym;
  int		idx;
  int		size = 0;
  char		*actual;
  elfshsect_t	*sect;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check arguments */
  if (file == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  sym = (elfsh_Sym *) elfsh_get_dynsymtab(file, &size);

  /* Check DYNSYM section data pointer */
  if (sym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get DYNSYM", NULL);

  sect = file->secthash[ELFSH_SECTION_DYNSYM];

  /* Check DYNSYM section pointer */
  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get DYNSYM section pointer", NULL);

  /*
  if (file->dynsymhash.ent)
    {
    //idx is the symbol number in the section
      idx = (int) hash_get(&file->dynsymhash, name);

#if __DEBUG_HASH_BY_NAME__
      printf("[DEBUG_HASH_BY_NAME] DYNSYM HASH Search by name for %s => %d\n",
	     name, idx);
#else
      //Check if idx is in the section
      if (idx <= 0 || idx >= sect->shdr->sh_size)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Symbol not found", NULL);
      
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym + idx));
#endif
    }
*/

  for (idx = 0; idx < size; idx++)
    {
      actual = elfsh_get_dynsymbol_name(file, sym + idx);
      if (actual && !strcmp(actual, name))
	{
#if __DEBUG_HASH_BY_NAME__
	  printf("[DEBUG_HASH_BY_NAME] DYNSYM ITERATE Search by name for %s => %d\n", 
		 name, idx);
#endif
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym + idx));
	}
    }

#if __DEBUG_HASH_BY_NAME__
  printf("[DEBUG_HASH_BY_NAME] DYNSYM ITERATE Search by name for %s => NOT FOUND\n", name);
#endif

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Symbol not found", NULL);
}



/**
 * Shift the dynamic symbol table, mostly useful on ET_DYN objects 
 * @param file
 * @param limit
 * @param inc
 * @return
 */
int		elfsh_shift_dynsym(elfshobj_t *file, eresi_Addr limit, int inc)
{
  elfshsect_t	*actual;
  int		err;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  actual = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTDYNSYM, 
				     NULL, NULL, NULL);
  if (!actual)
    {
      actual = elfsh_get_section_by_type(file, SHT_DYNSYM, 0, NULL, NULL, NULL);
      if (actual == NULL || actual->data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to find DYNSYM by type", -1);
    }

  err = elfsh_shift_syms(file, actual, limit, inc);
  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to shift DYNSYM", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
