/**
 * @file hash.c
 * @ingroup libelfsh
** hash.c for libelfsh
** 
** Started on  Mon Feb 26 04:15:44 2001 jfv
** 
**
** $Id: hash.c,v 1.13 2007-08-25 17:13:05 mxatone Exp $
**
*/
#include "libelfsh.h"

static void	*lastdata;

/**
 * Return the chain number 
 * @param chain chain element
 * @return num value
 */
elfsh_Word	elfsh_get_hashchain_num(elfsh_Word *chain)
{
  elfsh_Word    nchain;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (lastdata == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      " Cannot get HASH",  0);
  
  nchain = elfsh_get_hashnchain(lastdata);

  lastdata = NULL;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (nchain));
}

/**
 * Set a value for the chain number 
 * @param chain chain element
 * @param new num value
 */
int	     	elfsh_set_hashchain_num(elfsh_Word *chain, elfsh_Word val)
{
  elfsh_Word   	*c;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (lastdata == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  c = (elfsh_Word*) lastdata;

  *(c + 1) = val;

  lastdata = NULL;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the bucket number 
 * @param bucket bucket element
 * @return number value
 */
elfsh_Word	elfsh_get_hashbucket_num(elfsh_Word *bucket)
{
  elfsh_Word	nbucket;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (lastdata == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter",  0);
  
  nbucket = elfsh_get_hashnbucket(lastdata);

  lastdata = NULL;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (nbucket));
}

/**
 * Set a value for the bucket number 
 * @param bucket bucket element
 * @param val new number
 */
int	     	elfsh_set_hashbucket_num(elfsh_Word *bucket, elfsh_Word val)
{
  elfsh_Word   	*b;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (lastdata == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  b = (elfsh_Word*) lastdata;

  *b = val;

  lastdata = NULL;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return a value for a chain 
 * @param chain chain element 
 * @return value
 */
elfsh_Word   	elfsh_get_hashchain_value(elfsh_Word *chain)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (chain == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (*chain));
}

/**
 * Set a value for a chain value 
 * @param chain chain element
 * @param val new value
 */
int	       	elfsh_set_hashchain_value(elfsh_Word *chain, elfsh_Word val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (chain == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  *chain = val;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return a value for a bucket 
 * @param bucket bucket element
 * @return value
 */
elfsh_Word    	elfsh_get_hashbucket_value(elfsh_Word *bucket)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (bucket == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (*bucket));
}

/**
 * Set a value for a bucket value 
 * @param bucket bucket element
 * @param val new value
 */
int	     	elfsh_set_hashbucket_value(elfsh_Word *bucket, elfsh_Word val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (bucket == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  *bucket = val;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Get hash chain by name 
 * @param file target file 
 * @param sname symbol name
 * @return chain
 */
elfsh_Word 	*elfsh_get_hashchain_by_name(elfshobj_t *file, char *sname)
{
  elfsh_Sym	*sym;
  void		*data;
  elfsh_Word	*chain;
  elfsh_Word	nchain;
  unsigned int		index;
  elfsh_Word   	symid;
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_hashtable(file, NULL);
  if (data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      " Cannot get HASH",  0);

  sym = elfsh_get_dynsymtab(file, NULL);
  if (sym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get DYNSYM",  0);

  lastdata = data;

  chain = elfsh_get_hashchain(data);
  nchain = elfsh_get_hashnchain(data);

  for (index = 0; index < nchain; index++)
    {
      symid = chain[index];
      name = elfsh_get_dynsymbol_name(file, sym + symid);
      
      if (!strcmp(sname, name))
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (chain + index));  
    
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));  
}

/**
 * Get hash chain by index 
 * @param ps data pointer
 * @param index chain index
 * @return chain entry
 */
elfsh_Word     	*elfsh_get_hashchain_by_index(void *ps, unsigned int index)
{
  elfsh_Word  	*chain;
  elfsh_Word	nchain;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  chain = elfsh_get_hashchain(ps);
  nchain = elfsh_get_hashnchain(ps);

  lastdata = ps;

  if (index < nchain)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (chain + index));  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));  
}

/**
 * Get hash bucket by name 
 * @param file target file
 * @param symbol name
 * @return bucket
 */
elfsh_Word 		*elfsh_get_hashbucket_by_name(elfshobj_t *file, char *sname)
{
  elfsh_Sym	*sym;
  void		*data;
  elfsh_Word   	*bucket;
  elfsh_Word   	nbucket;
  int		index;
  elfsh_Word   	symid;
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_hashtable(file, NULL);
  if (data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      " Cannot get HASH",  0);

  sym = elfsh_get_dynsymtab(file, NULL);
  if (sym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get DYNSYM",  0);

  lastdata = data;

  bucket = elfsh_get_hashbucket(data);
  nbucket = elfsh_get_hashnbucket(data);

  for (index = 0; index < nbucket; index++)
    {
      symid = bucket[index];
      name = elfsh_get_dynsymbol_name(file, sym + symid);
      
      if (!strcmp(sname, name))
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (bucket + index));  
    
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));  
}

/**
 * Get hash bucket by index 
 * @param ps data pointer
 * @parma index bucket index
 * @return pointer on the bucket
 */
elfsh_Word     	*elfsh_get_hashbucket_by_index(void *ps, unsigned int index)
{
  elfsh_Word   	*bucket;
  elfsh_Word   	nbucket;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  bucket = elfsh_get_hashbucket(ps);
  nbucket = elfsh_get_hashnbucket(ps);

  lastdata = ps;

  if (index < nbucket)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (bucket + index));  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));  
}

/**
 * Get chain
 * @param data element pointer
 * @return value
 */
elfsh_Word     	*elfsh_get_hashchain(const void *data)
{
  elfsh_Word   	*bucket;
  elfsh_Word   	nbucket;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  nbucket = elfsh_get_hashnbucket(data);
  bucket  = elfsh_get_hashbucket(data);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ((elfsh_Word *) bucket + nbucket));
}

/**
 * Get n chain
 * @param data element pointer
 * @return value
 */
elfsh_Word     	elfsh_get_hashnchain(const void *data)
{
  elfsh_Word   	*p;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  p = (elfsh_Word *) data;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (*(p + 1)));
}

/**
 * Get bucket
 * @param data element pointer
 * @return value
 */
elfsh_Word     	*elfsh_get_hashbucket(const void *data)
{
  elfsh_Word   	*p;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  p = (elfsh_Word *) data;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (p + 2));
}

/**
 * Get n bucket
 * @param data element pointer
 * @return value
 */
elfsh_Word     	elfsh_get_hashnbucket(const void *data)
{
  elfsh_Word   	*p;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  p = (elfsh_Word *) data;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (*p));
}

/**
 * Return a pointer on the symbol hash table and load it if needed 
 * @param file target file
 * @param num number of entities (optional)
 * @return data pointer
 */
void		*elfsh_get_hashtable(elfshobj_t *file, int *num)
{
  elfshsect_t	*enew;
  int		nbr;
  void		*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (file->secthash[ELFSH_SECTION_HASH] == NULL)
    {
      enew = elfsh_get_section_by_type(file, SHT_HASH, NULL, NULL, &nbr, 0);
      if (enew == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get HASH by type", NULL);
      
      enew->data = elfsh_load_section(file, enew->shdr);
      if (!enew->data)	
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No Hash table",  NULL);

      file->secthash[ELFSH_SECTION_HASH] = enew;
    }  

  ret = elfsh_get_raw(file->secthash[ELFSH_SECTION_HASH]);

  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_HASH]->shdr->sh_size;
  nbr /= sizeof(elfsh_Word);

  if (num != NULL)
    *num = nbr;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/**
 * Return a pointer on a symbol hash table (depending of range value) and load it if needed 
 * @param file target file
 * @param range range index
 * @param num number of entities (optional)
 * @return section pointer
 */
elfshsect_t	*elfsh_get_hashtable_by_range(elfshobj_t *file, eresi_Addr range, int *num)
{
  elfshsect_t	*new;
  int		nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  new = elfsh_get_section_by_type(file, SHT_HASH, range, NULL, &nbr, 0);
  if (new == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get HASH by type", NULL);

  if (!new->data)
    {
      new->data = elfsh_load_section(file, new->shdr);
      if (!new->data)	
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "No Hash table",  NULL);
    }
  
  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_HASH]->shdr->sh_size;
  nbr /= sizeof(elfsh_Word);

  if (num != NULL)
    *num = nbr;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, new);
}

/**
 * Return the symbol value giving its name using the symbol hash table 
 * @param file target file
 * @param name symbol name
 * @return symbol value or 0
 */
int		elfsh_get_dynsymbol_by_hash(elfshobj_t *file, char *name)
{
  elfsh_Sym	*sym;
  void		*data;
  char		*sname;
  elfsh_Word   	hash;
  elfsh_Word   	nbucket;
  elfsh_Word   	nchain;
  elfsh_Word   	*chain;
  elfsh_Word   	*bucket;
  int		index;
  elfsh_Word   	symid;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_hashtable(file, NULL);
  if (data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      " Cannot get HASH",  0);

  sym = elfsh_get_dynsymtab(file, NULL);
  if (sym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get DYNSYM",  0);

  hash    = elfsh_get_symbol_hash(name);
  nbucket = elfsh_get_hashnbucket(data);
  nchain  = elfsh_get_hashnchain(data);
  bucket  = elfsh_get_hashbucket(data);
  chain   = elfsh_get_hashchain(data);
  
  index = bucket[hash % nbucket];
  sname = elfsh_get_dynsymbol_name(file, sym + index);

  if (!strcmp(name, sname))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym[index].st_value));

  /* 
  ** The first fetched dynamic symbol isnt the one we are looking for
  ** so we loop on the entry .
  */
  for (sym = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSYM]); 
       index < nchain; index = symid)
    {

#if __DEBUG__
      printf("[LIBELFSH] Hash LOOP on bucket [%u] \n", (unsigned int) index);
#endif

      if (chain[index] == STN_UNDEF)
	break;

      symid = chain[index];
      
      sname = elfsh_get_dynsymbol_name(file, &sym[symid]);
      if (!strcmp(name, sname))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym[chain[index]].st_value));
    }
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "No Hash found for the symbol",  0);
}

/**
 * Pick up a version definition entry from a name
 * @param file host file
 * @param name function name
 * @param defdata version def section data pointer
 * @param size section size
 * @return a pointer on version definition entry
 */
elfsh_Verdef  	*elfsh_hash_getdef(elfshobj_t *file, char *name, void *defdata, int size)
{
  elfsh_Verdef	*table;
  elfsh_Half	*sym;
  unsigned int		offset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);

  /* We can get elements if we do not already have them */
  if (defdata == NULL)
    {
      defdata = elfsh_get_verdeftab(file, &size);

      if (defdata == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Can't find version def section", NULL);
    }

  /* Get version symbol to check that is a def and not a need*/
  sym = elfsh_get_versym_by_name(file, name);

  if (sym)
    {
      /* Search on the verdef table */
      for (offset = 0; offset < size; offset += table->vd_next)
	{
	  table = defdata + offset;

	  /* This symbol was a definition */
	  if (table->vd_ndx == *sym)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, table);

	  if (table->vd_next == 0)
	    break;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/**
 * Check if this file match directly
 * @param file target file
 * @param name function name
 * @return file object
 */
static elfshobj_t	*elfsh_hash_getfile_def_direct(elfshobj_t *file, char *name)
{
  int			defsize;
  void			*defdata;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check if a version def section exist else we do nothing */
  defdata = elfsh_get_verdeftab(file, &defsize);

  if (defdata)
    {
      /* Try on the current file */
      if (elfsh_get_dynsymbol_by_hash(file, name) > 0
	  && elfsh_hash_getdef(file, name, defdata, defsize))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, file);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/**
 * Research a function on dependences file using hash version
 * @param file target file
 * @param name function name
 * @return file object
 */
elfshobj_t	*elfsh_hash_getfile_def(elfshobj_t *file, char *name)
{
  char		**keys;
  unsigned int		index;
  int		keynbr;
  elfshobj_t	*getfile;
#if defined(sun)
  Link_map	*actual;
#else
  elfshlinkmap_t *actual;
#endif
  char		*fname;
  hash_t	*filehash;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We need original file to use child_hash */
  if (file->original)
    file = file->original;

  /* Check directly this file */
  if (elfsh_hash_getfile_def_direct(file, name))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, file);

  /* Debug mode use a different dependences technique for mapped files */
  if (elfsh_is_debug_mode())
    {
      filehash = hash_find("files");

      if (!filehash)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Can't find file list", NULL);

      /* Iterate linkmap list */
      for (actual = elfsh_linkmap_get_lnext(file->linkmap);
	   actual != NULL; 
	   actual = elfsh_linkmap_get_lnext(actual))
	{
	  fname = elfsh_linkmap_get_lname(actual);

	  if (fname && *fname)
	    {
	      getfile = (elfshobj_t *) hash_get(filehash, fname);

	      if (getfile)
		{
		  /* Check directly this file */
		  if (elfsh_hash_getfile_def_direct(getfile, name))
		    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, getfile);
		}
	    }
	}
    }
  else /* Use static file dependence hash table */
    {
      if (hash_size(&file->child_hash))
	{
	  /* Search on his childs */
	  keys = hash_get_keys(&file->child_hash, &keynbr);
	  if (keys)
	    {
	      for (index = 0; index < keynbr; index++)
		{
		  getfile = (elfshobj_t *) hash_get(&file->child_hash, keys[index]);
		  if (getfile)
		    {
		      getfile = elfsh_hash_getfile_def(getfile, name);
		      
		      if (getfile)
			PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, getfile);
		    }
		}
	      
	      hash_free_keys(keys);
	    }
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/**
 * Get the hash for the symbol name . Adapted from the ELF TIS reference 
 * @param name input name to hash
 * @return hash value
 */
elfsh_Word	elfsh_get_symbol_hash(char *name)
{
  unsigned long h;
  unsigned long g;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (h = 0; *name; h &= ~g)
    {
      h = (h << 4) + *name++;
      if ((g = h & 0xF0000000))
	h ^= g >> 24;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_Word) (h));
}

























