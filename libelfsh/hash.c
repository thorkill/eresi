/*
** hash.c for libelfsh
** 
** Started on  Mon Feb 26 04:15:44 2001 mayhem
**
*/
#include "libelfsh.h"



/* Return a pointer on the symbol hash table and load it if needed */
void		*elfsh_get_hashtable(elfshobj_t *file)
{
  elfshsect_t	*new;
  int		nbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (file->secthash[ELFSH_SECTION_HASH] == NULL)
    {

      new = elfsh_get_section_by_type(file, SHT_HASH, NULL, NULL, &nbr, 0);
      if (!new)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get HASH by type", NULL);
      
      new->data = elfsh_load_section(file, new->shdr);
      if (!new->data)	
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No Hash table",  NULL);

      file->secthash[ELFSH_SECTION_HASH] = new;
    }
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_raw(file->secthash[ELFSH_SECTION_HASH])));
}






/* Return the symbol value giving its name using the symbol hash table */
int		elfsh_get_dynsymbol_by_hash(elfshobj_t *file, char *name)
{
  elfsh_Sym	*sym;
  char		*sname;
  int		hash;
  int		nbucket;
  int		nchain;
  int		*chain;
  int		*bucket;
  int		index;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_HASH] == NULL && 
      elfsh_get_hashtable(file) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      " Cannot get HASH",  0);

  if (NULL == file->secthash[ELFSH_SECTION_DYNSYM] && 
      NULL == elfsh_get_dynsymtab(file, NULL))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get DYNSYM",  0);

  hash    = elfsh_get_symbol_hash(name);
  nbucket = *(int *)  elfsh_get_raw(file->secthash[ELFSH_SECTION_HASH]);
  nchain  = *((int *) file->secthash[ELFSH_SECTION_HASH]->data + 1);
  bucket  = (int *)   file->secthash[ELFSH_SECTION_HASH]->data + 2;
  chain   = (int *)   bucket + nbucket;
  
  index = bucket[hash % nbucket];
  sym = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSYM]);
  sname = elfsh_get_dynsymbol_name(file, sym + index);

  if (!strcmp(name, sname))
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym[index].st_value));

  /* 
  ** The first fetched dynamic symbol isnt the one we are looking for
  ** so we loop on the entry .
  */
  for (sym = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSYM]); 
       index < nchain; index++)
    {

#if __DEBUG__
      printf("[LIBELFSH] Hash LOOP on bucket [%u] \n", (u_int) index);
#endif
      
      sname = elfsh_get_dynsymbol_name(file, &sym[chain[index]]);
      if (!strcmp(name, sname))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym[chain[index]].st_value));
    }
  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "No Hash found for the symbol",  0);
}






/* Get the hash for the symbol name . Adapted from the ELF TIS reference */
int		elfsh_get_symbol_hash(char *name)
{
  unsigned long h;
  unsigned long g;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (h = 0; *name; h &= ~g)
    {
      h = (h << 4) + *name++;
      if ((g = h & 0xF0000000))
	h ^= g >> 24;
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (h));
}

























