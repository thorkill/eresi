/*
** hash.c for libelfsh
** 
** Started on  Mon Feb 26 04:15:44 2001 mayhem
**
*/
#include "libelfsh.h"

static void	*lastdata;

/* Return the chain number */
int	       	elfsh_get_hashchain_num(int *chain)
{
  int		nchain;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (lastdata == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      " Cannot get HASH",  0);
  
  nchain = elfsh_get_hashnchain(lastdata);

  lastdata = NULL;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (nchain));
}

/* Set a value for the chain number */
int	     	elfsh_set_hashchain_num(int *chain, int val)
{
  int		*c;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (lastdata == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  c = (int*) lastdata;

  *(c + 1) = val;

  lastdata = NULL;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Return the bucket number */
int	       	elfsh_get_hashbucket_num(int *bucket)
{
  int		nbucket;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (lastdata == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter",  0);
  
  nbucket = elfsh_get_hashnbucket(lastdata);

  lastdata = NULL;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (nbucket));
}

/* Set a value for the bucket number */
int	     	elfsh_set_hashbucket_num(int *bucket, int val)
{
  int		*b;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (lastdata == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  b = (int*) lastdata;

  *b = val;

  lastdata = NULL;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Return a value for a chain */
int	       	elfsh_get_hashchain_value(int *chain)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (chain == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (*chain));
}

/* Set a value for a chain value */
int	       	elfsh_set_hashchain_value(int *chain, int val)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (chain == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  *chain = val;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Return a value for a bucket */
int	       	elfsh_get_hashbucket_value(int *bucket)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (bucket == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (*bucket));
}

/* Set a value for a bucket value */
int	     	elfsh_set_hashbucket_value(int *bucket, int val)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (bucket == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  *bucket = val;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Get hash chain by name */
int 		*elfsh_get_hashchain_by_name(elfshobj_t *file, char *sname)
{
  elfsh_Sym	*sym;
  void		*data;
  int		*chain;
  int		nchain;
  int		index;
  int		symid;
  char		*name;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_hashtable(file, NULL);
  if (data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      " Cannot get HASH",  0);

  sym = elfsh_get_dynsymtab(file, NULL);
  if (sym == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get DYNSYM",  0);

  lastdata = data;

  chain = elfsh_get_hashchain(data);
  nchain = elfsh_get_hashnchain(data);

  for (index = 0; index < nchain; index++)
    {
      symid = chain[index];
      name = elfsh_get_dynsymbol_name(file, sym + symid);
      
      if (!strcmp(sname, name))
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (chain + index));  
    
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));  
}

/* Get hash chain by index */
int       	*elfsh_get_hashchain_by_index(void *ps, int index)
{
  int		*chain;
  int		nchain;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  chain = elfsh_get_hashchain(ps);
  nchain = elfsh_get_hashnchain(ps);

  lastdata = ps;

  if (index < nchain)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (chain + index));  

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));  
}

/* Get hash bucket by name */
int 		*elfsh_get_hashbucket_by_name(elfshobj_t *file, char *sname)
{
  elfsh_Sym	*sym;
  void		*data;
  int		*bucket;
  int		nbucket;
  int		index;
  int		symid;
  char		*name;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_hashtable(file, NULL);
  if (data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      " Cannot get HASH",  0);

  sym = elfsh_get_dynsymtab(file, NULL);
  if (sym == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get DYNSYM",  0);

  lastdata = data;

  bucket = elfsh_get_hashbucket(data);
  nbucket = elfsh_get_hashnbucket(data);

  for (index = 0; index < nbucket; index++)
    {
      symid = bucket[index];
      name = elfsh_get_dynsymbol_name(file, sym + symid);
      
      if (!strcmp(sname, name))
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (bucket + index));  
    
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));  
}

/* Get hash bucket by index */
int       	*elfsh_get_hashbucket_by_index(void *ps, int index)
{
  int		*bucket;
  int		nbucket;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  bucket = elfsh_get_hashbucket(ps);
  nbucket = elfsh_get_hashnbucket(ps);

  lastdata = ps;

  if (index < nbucket)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (bucket + index));  

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));  
}

int		*elfsh_get_hashchain(const void *data)
{
  int		*bucket;
  int		nbucket;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  nbucket = elfsh_get_hashnbucket(data);
  bucket  = elfsh_get_hashbucket(data);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ((int *) bucket + nbucket));
}

int		elfsh_get_hashnchain(const void *data)
{
  int 		*p;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  p = (int *) data;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (*(p + 1)));
}


int		*elfsh_get_hashbucket(const void *data)
{
  int		*p;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  p = (int *) data;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (p + 2));
}

int		elfsh_get_hashnbucket(const void *data)
{
  int		*p;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  p = (int *) data;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (*p));
}

/* Return a pointer on the symbol hash table and load it if needed */
void		*elfsh_get_hashtable(elfshobj_t *file, int *num)
{
  elfshsect_t	*new;
  int		nbr;
  void		*ret;
  int		nbucket;
  int		nchain;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (file->secthash[ELFSH_SECTION_HASH] == NULL)
    {

      new = elfsh_get_section_by_type(file, SHT_HASH, NULL, NULL, &nbr, 0);
      if (new == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get HASH by type", NULL);
      
      new->data = elfsh_load_section(file, new->shdr);
      if (!new->data)	
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No Hash table",  NULL);

      file->secthash[ELFSH_SECTION_HASH] = new;
    }  

  ret = elfsh_get_raw(file->secthash[ELFSH_SECTION_HASH]);

  nbucket = elfsh_get_hashnbucket(ret);
  nchain = elfsh_get_hashnchain(ret);

  if (num != NULL)
    *num = nbucket+nchain;
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

/* Return the symbol value giving its name using the symbol hash table */
int		elfsh_get_dynsymbol_by_hash(elfshobj_t *file, char *name)
{
  elfsh_Sym	*sym;
  void		*data;
  char		*sname;
  int		hash;
  int		nbucket;
  int		nchain;
  int		*chain;
  int		*bucket;
  int		index;
  int		symid;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_hashtable(file, NULL);
  if (data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      " Cannot get HASH",  0);

  sym = elfsh_get_dynsymtab(file, NULL);
  if (sym == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get DYNSYM",  0);

  hash    = elfsh_get_symbol_hash(name);
  nbucket = elfsh_get_hashnbucket(data);
  nchain  = elfsh_get_hashnchain(data);
  bucket  = elfsh_get_hashbucket(data);
  chain   = elfsh_get_hashchain(data);
  
  index = bucket[hash % nbucket];
  sname = elfsh_get_dynsymbol_name(file, sym + index);

  if (!strcmp(name, sname))
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym[index].st_value));

  /* 
  ** The first fetched dynamic symbol isnt the one we are looking for
  ** so we loop on the entry .
  */
  for (sym = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSYM]); 
       index < nchain; index = symid)
    {

#if __DEBUG__
      printf("[LIBELFSH] Hash LOOP on bucket [%u] \n", (u_int) index);
#endif

      if (chain[index] == STN_UNDEF)
	break;

      symid = chain[index];
      
      sname = elfsh_get_dynsymbol_name(file, &sym[symid]);
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

























