/**
* @file libaspect/libhash.c
 * @ingroup libaspect
 * 
 * @brief Contain ELFsh internal hashtables library calls.
 *
 * Started on  Fri Jan 24 20:26:18 2003 jfv
 * $Id$
 */
#include "libaspect.h"


/* Hash tables of hash tables */
hash_t  *hash_hash = NULL;

/** 
 * @brief Initialize the hash table 
 * @param h Pointer to the hash to initialize
 * @param name Name of the hash.
 * @param size Size to document
 * @param type Type to document
 * @param Returns 0 on success, -1 on error or 1 if hash already exists.
 */
int hash_init(hash_t *h, char *name, int size, u_int type)
{
  NOPROFILER_IN();

  /* First checks */
  /* Initialize the global hash table of lists and 
     the global hash table of hash tables */
  if (!hash_hash)
    {
      hash_hash = (hash_t *) calloc(sizeof(hash_t), 1);
      hash_init(hash_hash, "hashes", 51, ASPECT_TYPE_UNKNOW);
    }
  if (type >= aspect_type_nbr)
    {
      fprintf(stderr, "Unable to initialize hash table %s \n", name);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to initialize hash table", -1);
    }
  if (h != hash_hash && hash_find(name) && h->ent)
    {
#if __DEBUG__
      fprint(stderr, "Hash table already exists and initialized\n");
#endif
      NOPROFILER_ROUT(1);
    }

  //printf("INIT HASH %s \n", name);
  
  /* Add a new element */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 h->ent, size * sizeof(listent_t), -1);
  h->size      = size;
  h->type      = type;
  h->elmnbr    = 0;
  h->linearity = 0;
  h->name      = name;
  hash_add(hash_hash, name, h);

  if (!hash_lists)
    {
      hash_lists = (hash_t *) calloc(sizeof(hash_t), 1);
      hash_init(hash_lists, "lists", 51, ASPECT_TYPE_UNKNOW);
    }
  NOPROFILER_ROUT(0);
}

/** 
 * @brief Return a hash table by its name 
 * @param name Name of the hash to retrieve.
 * @return A pointer to a hash_t structure  or NULL on error.
 */
hash_t  *hash_find(char *name)
{
  if (!name)
    return (NULL);
  return ((hash_t *) hash_get(hash_hash, name));
}

/**
 * Return a hash table pointer by its name.
 * Overwrite existing table if there was one sharing that name, only
 *  if both tables have the same elements type 
 * @param table
 * @param name
 * @return
 */
int		hash_register(hash_t *table, char *name)
{
  hash_t	*h;
  int		sz;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  h = hash_get(hash_hash, name);
  if (h)
    {
      if (h->type == ASPECT_TYPE_UNKNOW)
	h->type = table->type;
      if (h->type != table->type)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Incompatible hash tables", -1);
      if (h->elmnbr)
	h = hash_empty(name);
      hash_merge(h, table);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }  
  XALLOC(__FILE__, __FUNCTION__, __LINE__, h, sizeof(hash_t), -1);
  sz = (table->size > table->elmnbr ? table->size : table->elmnbr);
  hash_init(h, name, sz, table->type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * @brief Empty a hash table 
 * @param name Name of the ash table to empty.
 * @param returrn a pointer to the hash table or NULL on error.
 */
hash_t    *hash_empty(char *name)
{
  hash_t  *hash;
  char  *newname;
  int	  size;
  char    type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  hash = hash_find(name);
  if (!hash)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);

  //printf("EMPTY HASH %s \n", name);

  size    = hash->size;
  type    = hash->type;
  hash_del(hash_hash, name);
  hash_destroy(hash);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
   newname, strlen(name) + 1, NULL);
  strncpy(newname, name, strlen(name));
  hash_init(hash, newname, size, type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, hash);
}



/** 
 * Destroy a hash table.
 * @param hash Pointer to the hash to destroy
 */
void		hash_destroy(hash_t *hash)
{
  char		**keys;
  int		idx;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We should not destroy the elements as they might be in other hashes */
  keys = hash_get_keys(hash, &keynbr);
  for (idx = 0; idx < keynbr; idx++)
    if (keys[idx])
      XFREE(__FILE__, __FUNCTION__, __LINE__, keys[idx]);
  if (keys)
    hash_free_keys(keys);
  hash_del(hash_hash, hash->name);
  XFREE(__FILE__, __FUNCTION__, __LINE__, hash->ent);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/** 
 * @brief Add an entry to the hash table.
 * @param h Hash table.
 * @param key Key of the new entry.
 * @param data Value associated with the key.
 * @return Returns an index (to document)
 */
int		hash_add(hash_t *h, char *key, void *data)
{
  listent_t	*actual;
  listent_t	*newent;
  char		*backup;
  u_int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //Weaken the check : do not hash_get(h, key) check and do not check !data
  if (!h || !key)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", -1);

  /* If the element already exist, make sure we erase the existing one */
  actual = hash_get(h, key);
  if (actual)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		  hash_set(h, key, data));

  newent = NULL;
  for (index = 0, backup = key; *backup; backup++)
    index += *backup;
  index %= h->size;

  if (h->ent[index].key == NULL)
    {
      h->ent[index].key  = key;
      h->ent[index].data = data;
    }
  else
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     newent, sizeof(listent_t), -1);
      newent->key  = key;
      newent->data = data;
      actual = h->ent + index;
      while (actual->next)
	actual = actual->next;
      actual->next = newent;
    }
  h->elmnbr++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, index);
}

/**
 * @brief Delete an entry from the hash table.
 * @param
 */
int		hash_del(hash_t *h, char *key)
{
  listent_t	*actual;
  listent_t	*todel;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check the first entry for this hash */
  actual = hash_get_head(h, key);
  if (actual->key != NULL && !strcmp(actual->key, key))
    {
      if (actual->next)
	{
	  todel = actual->next;
	  *actual = *actual->next;
	  XFREE(__FILE__, __FUNCTION__, __LINE__, todel);
	}
      else
	bzero(actual, sizeof (listent_t));
      h->elmnbr--;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* Looking for the good entry in the list for this hash value */
  while (actual->next      != NULL && 
	 actual->next->key != NULL && 
	 strcmp(actual->next->key, 
		key))
    actual = actual->next;
  
  /* Not found */
  if (!actual->next)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
  
  /* Found */
  todel = actual->next;
  actual->next = actual->next->next;
  XFREE(__FILE__, __FUNCTION__, __LINE__, todel);
  h->elmnbr--;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Retrieve the metadata for a given key 
 *
 * @param h
 * @param key
 */
void		*hash_get(hash_t *h, char *key)
{
  listent_t	*actual;

  actual = hash_get_head(h, key);
  while (actual      != NULL && 
	 actual->key != NULL && 
	 strcmp(actual->key, 
		key))
    actual = actual->next;
  return (actual != NULL ? actual->data : NULL);
}

/** 
 * Retrieve the data pointer for a given key 
 *
 * @param h
 * @param key
 */
void		*hash_select(hash_t *h, char *key)
{
  listent_t	*actual;

  actual = hash_get_head(h, key);
  while (actual      != NULL && 
   actual->key != NULL && 
   strcmp(actual->key, key))
    actual = actual->next;
  return (actual != NULL ? &actual->data : NULL);
}



/**
 * Change the metadata for an existing entry, giving its key 
 * @param h
 * @param key
 * @param data
 * @return
 */
int		hash_set(hash_t *h, char *key, void *data)
{
  listent_t	*ent;

  ent = hash_get_ent(h, key);
  if (!ent || (!ent->key && !ent->data))
    return (hash_add(h, key, data));
  ent->data = data;
  return (0);
}


/**
 * @brief Retrieve the -entry- for a given key 
 * @param h
 * @param key
 * @return
 */
listent_t   *hash_get_ent(hash_t *h, char *key)
{
  listent_t *actual;
  
  actual = hash_get_head(h, key);
  while (actual      != NULL && 
	 actual->key != NULL && 
	 strcmp(actual->key, key))
    actual = actual->next;
  return (actual);
}

/** 
 * Retreive a Hash entry head giving the key *
 * @param h
 * @param backup
 * @return
 */
listent_t *hash_get_head(hash_t *h, char *backup)
{
  u_int   index;

  for (index = 0; *backup; backup++)
    index += *backup;
  return (&h->ent[index % h->size]);
}



/** 
 * @brief Used to create arrays of keys for completion 
 * @param h
 * @param n
 * @return
 */
char		**hash_get_keys(hash_t *h, int *n)
{
  char		**keys;
  listent_t	*curelem;
  int		idx;
  int		last;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !h->elmnbr)
    {
      if (n) 
	*n = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid NULL parameters", NULL);
    }
  XALLOC(__FILE__, __FUNCTION__, __LINE__, keys, 
	 sizeof(char *) * (h->elmnbr + 1), NULL);

  for (last = idx = 0; idx < h->size; idx++)
    {
      curelem = h->ent + idx;
      while (curelem && curelem->key)
	{
	  keys[last] = curelem->key;
	  curelem = curelem->next;
	  last++;
	}
    }
  if (n)
    *n = h->elmnbr;
  keys[last] = NULL;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, keys);
}


/** 
 * @brief Free the keys returned by hash_get_keys() 
 */
void    hash_free_keys(char **keys)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (keys)
    XFREE(__FILE__, __FUNCTION__, __LINE__, keys);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Print the hash table (DEBUG PURPOSE) 
 * @param h Hash table.
 */
void            hash_print(hash_t *h)
{
  listent_t     *actual;
  int           index;

  if (!h)
    return;
  puts(".::. Printing hash .::. ");
  for (index = 0; index < h->size; index++)
    {
      for (actual = &h->ent[index]; 
	   actual != NULL && actual->key != NULL; 
	   actual = actual->next)
	printf(" ENT [%u] key = %s ; data = %p ; next = %p\n",
	       index, actual->key, actual->data, actual->next);
    }
}

/** 
 * @brief Apply func all entries .
 * @param h
 * @param ptr
 * @param func
 * @return
*/
int   hash_apply(hash_t      *h, 
 		 void        *ptr, 
		 int         (*func)(listent_t *ph, void *pptr))
{
  listent_t     *actual;
  int           index;
  int		ret = 0;
  
  for (index = 0; index < h->size; index++)
    {
      for (actual = &h->ent[index];
           actual != NULL && actual->key != NULL;
           actual = actual->next)
	ret |= func (actual, ptr);
    }
  return ret;
}


/* Compare 2 hash tables */
/* Contributed by zorgon */
/* Can be used to compare ELF in memory and ELF in file */
int		hash_compare(hash_t *first, hash_t *two)
{
  int		index;
  int		m;
  listent_t	*actual;
  listent_t	*bis;

  if (first->size != two->size) 
    return (-1);
  for (m = index = 0; index < first->size; index++) 
    {
      for (actual = first->ent + index; 
	   actual != NULL && actual->key != NULL; 
	   actual = actual->next) 
	{
	  bis = hash_get_ent(two, actual->key);
	  if (actual->data != bis->data) 
	    {
	      printf("FIRST  key = %s ; data = %p", 
		     actual->key, actual->data);
	      printf("SECOND key = %s ; data = %p", 
		     bis->key, bis->data);
	      m++;
	    }
	}
    }
  if (m)
    return (-1);
  return (0);
}


/* Merge hash tables in the first one */
/* We cannot use hash_get_keys() because we dont know the type of hashed objects */
int		hash_merge(hash_t *dst, hash_t *src)
{
  listent_t     *actual;
  int           index;
  int		ret;

  /* Make sure we dont inject element already presents */
  if (!src || !dst || src->elmnbr == 0)
    return (0);
  for (ret = index = 0; index < src->size; index++)
    for (actual = &src->ent[index];
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      if (!hash_get(dst, actual->key))
	ret += hash_add(dst, actual->key, actual->data);
  return ret;
}



/* Intersect hash tables in the first one */
int		hash_inter(hash_t *dst, hash_t *src)
{
  char		**keys;
  int		keynbr;
  int		idx;
  char		*curkey;
  int		ret;

  if (!src || !dst || src->elmnbr == 0 || dst->elmnbr == 0)
    return (0);
  keys = hash_get_keys(dst, &keynbr);
  for (ret = idx = 0; idx < keynbr; idx++)
    {
      curkey = keys[idx];
      if (!hash_get(src, curkey))
	ret += hash_del(dst, curkey);
    }
  return ret;
}


/* Delete all elements of source hash in destination hash */
/* We cannot use hash_get_keys() because we dont know the type of hashed objects */
int		hash_unmerge(hash_t *dst, hash_t *src)
{
  listent_t	*actual;
  int           index;
  int		ret;
  
  /* Make sure we dont inject element already presents */
  if (!src || !dst || src->elmnbr == 0)
    return (0);
  for (ret = index = 0; index < src->size; index++)
    for (actual = &src->ent[index];
   actual != NULL && actual->key != NULL;
   actual = actual->next)
      if (hash_get(dst, actual->key))
  ret += hash_del(dst, actual->key);
  return ret;
}


/* Return the hash size */
int   hash_size(hash_t *hash)
{
  return (hash ? hash->elmnbr : 0);
}

/**
 * @brief Return the only element of this hash .
 * @param hash Hash table.
 * @return NULL on error.
 */
void		*hash_get_single(hash_t *hash)
{
  char		**keys;
  int		idx;

  if (!hash || hash_size(hash) != 1)
    return (NULL);
  keys = hash_get_keys(hash, &idx);
  return (hash_get(hash, keys[0]));
}

/**
 * Return an element of this hash 
 * The choice is non-deterministic.
 *
 * @param hash
 * @return
 */
void*		hash_get_one(hash_t *hash)
{
  char		**keys;
  int		index;

  if (!hash || !hash_size(hash))
    return (NULL);
  keys = hash_get_keys(hash, &index);
  return (hash_get(hash, keys[0]));
}


/**
 * Linear typing of list API.
 * @param h Hash table.
 */
u_char		hash_linearity_get(hash_t *h)
{
  if (!h)
    return (0);
  return (h->linearity);
}

/** 
 * Linear typing of list API .
 * @param h
 * @param val
 */
void		hash_linearity_set(hash_t *h, u_char val)
{
  if (!h)
    return;
  h->linearity = val;
}
