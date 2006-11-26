/*
** libhash.c for libelfsh
** 
** Contain ELFsh internal hashtables library calls
**
** Started on  Fri Jan 24 20:26:18 2003 mayhem
** Last update Mon Aug 15 11:47:50 2005 mayhem
*/
#include "libelfsh.h"


/* Initialize the hash table */
int	hash_init(hash_t *h, int size)
{
  HASHALLOC(h->ent, size * sizeof(hashent_t), -1);
  h->size   = size;
  h->elmnbr = 0;
  return (0);
}


/* Destroy a hash table */
void	hash_destroy(hash_t *h)
{
  XFREE(h->ent);
}



/* Add an entry to the hash table */
int		hash_add(hash_t *h, char *key, void *data)
{
  hashent_t	*actual;
  hashent_t	*new;
  char		*backup;
  u_int		index;

  new = NULL;
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
      HASHALLOC(new, sizeof(hashent_t), -1);
      new->key  = key;
      new->data = data;
      actual = h->ent + index;
      while (actual->next)
	actual = actual->next;
      actual->next = new;
    }
  h->elmnbr++;
  return (0);
}


/* Delete an entry from the hash table */
int		hash_del(hash_t *h, char *key)
{
  hashent_t	*actual;
  hashent_t	*todel;

  /* Check the first entry for this hash */
  //printf("before h = %p key = %p (%s) \n", h, key, (key ? key : ""));
  actual = hash_get_head(h, key);
  //printf("after  h = %p key = %p (%s) \n", h, key, (key ? key : ""));

  if (actual->key != NULL && !strcmp(actual->key, key))
    {
      if (actual->next)
	{
	  todel = actual->next;
	  *actual = *actual->next;
	  XFREE(todel);
	}
      else
	bzero(actual, sizeof (hashent_t));
      h->elmnbr--;
      return (0);
    }

  /* Looking for the good entry in the list for this hash value */
  while (actual->next      != NULL && 
	 actual->next->key != NULL && 
	 strcmp(actual->next->key, key))
    actual = actual->next;

  /* Not found */
  if (!actual->next)
    return (-1);

  /* Found */
  todel = actual->next;
  actual->next = actual->next->next;
  XFREE(todel);
  h->elmnbr--;
  return (0);
}



/* Retrieve the metadata for a given key */
void 		*hash_get(hash_t *h, char *key)
{
  hashent_t	*actual;

  //printf("before h = %p key = %p (%s) \n", h, key, (key ? key : ""));
  actual = hash_get_head(h, key);
  //printf("after  h = %p key = %p (%s) \n", h, key, (key ? key : ""));

  while (actual      != NULL && 
	 actual->key != NULL && 
	 strcmp(actual->key, key))
    actual = actual->next;
  return (actual != NULL ? actual->data : NULL);
}



/* Change the metadata for an existing entry, giving its key */
int		hash_set(hash_t *h, char *key, void *data)
{
  hashent_t	*ent;

  ent = hash_get_ent(h, key);
  if (!ent)
    return (-1);
  ent->data = data;
  return (0);
}


/* Retrieve the -entry- for a given key */
hashent_t 	*hash_get_ent(hash_t *h, char *key)
{
  hashent_t	*actual;

  actual = hash_get_head(h, key);
  while (actual      != NULL && 
	 actual->key != NULL && 
	 strcmp(actual->key, key))
    actual = actual->next;
  return (actual);
}



/* Retreive a Hash entry head giving the key */
hashent_t	*hash_get_head(hash_t *h, char *backup)
{
  u_int		index;

  for (index = 0; *backup; backup++)
    index += *backup;
  return (&h->ent[index % h->size]);
}



/* Used to create arrays of keys for completion */
char		**hash_get_keys(hash_t *h, int *n)
{
  int		j, i;
  char		**keys=NULL;
  hashent_t	*entry;
  
  for (i = j = 0; j < h->size; j++) 
    {
      entry = &h->ent[j];
      while (entry != NULL && entry->key != NULL) 
	{
	  
#if __DEBUG__
	  printf("hash[%u:%u] key = %s\n", j, i, entry->key);
#endif
	  keys = elfsh_realloc(keys, (i + 1) * sizeof(char *));
	  keys[i] = entry->key;
	  entry = entry->next;
	  i++;
	}
    }
  
  keys = elfsh_realloc(keys, (i + 1) * sizeof(char *));
  keys[i] = NULL;
  *n = i;
  return (keys);
}


/* Print the hash table (DEBUG PURPOSE) */
void            hash_print(hash_t *h)
{
  hashent_t     *actual;
  int           index;

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

/* Apply func all entries */
int		hash_apply(hash_t	*h, 
			   void		*ptr, 
			   int		(*func)(hashent_t *ph, void *pptr))
{
  hashent_t     *actual;
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
/* Contributed by zorgon, can be used to compare ELF in memory and ELF in file */
int		hash_compare(hash_t *first, hash_t *two)
{
  int		index;
  int		m;
  hashent_t	*actual;
  hashent_t	*bis;

  if (first->size != two->size) 
    return (-1);
  
  for (m = index = 0; index < first->size; index++) 
    {
      for (actual = &first->ent[index]; actual != NULL && actual->key != NULL; actual = actual->next) 
	{
	  bis = hash_get_ent(two, actual->key);
	  if (actual->data != bis->data) 
	    {
	      printf("FIRST  key = %s ; data = %p", actual->key, actual->data);
	      printf("SECOND key = %s ; data = %p", bis->key, bis->data);
	      m++;
	    }
	  //misused API
	  //hash_destroy(bis);
	}
    }
  if (m)
    return (-1);
  return (0);
}


/* Merge hash tables in the first one */
/* We cannot use hash_get_keys() because we dont know the type of hashed objects */
int		hash_merge(hash_t	*dst,
			   hash_t	*src)
{
  hashent_t     *actual;
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


/* Merge hash tables in the first one */
/* We cannot use hash_get_keys() because we dont know the type of hashed objects */
int		hash_unmerge(hash_t	*dst,
			     hash_t	*src)
{
  hashent_t	*actual;
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
int		hash_size(hash_t *hash)
{
  return (hash ? hash->elmnbr : 0);
}

/* Return the only element of this hash */
void*		hash_get_single(hash_t *hash)
{
  char		**keys;
  int		idx;

  if (!hash || hash_size(hash) != 1)
    return (NULL);
  keys = hash_get_keys(hash, &idx);
  return (hash_get(hash, keys[0]));
}

/* Return an element of this hash */
/* The choice is non-deterministic */
void*		hash_get_one(hash_t *hash)
{
  char		**keys;
  int		index;

  if (!hash || !hash_size(hash))
    return (NULL);
  keys = hash_get_keys(hash, &index);
  return (hash_get(hash, keys[0]));
}
