/**
 * @file libaspect/liblist.c
 * @ingroup libaspect
 * 
 * @brief Contain ELFsh internal lists related API.
 *
 * Started on  Fri Jul 13 20:26:18 2007 jfv
 * $Id$
 */

#include "libaspect.h"

/* Hash tables of hash tables */
hash_t  *hash_lists = NULL;

/**
 * @brief Initialize the hash table
 */
int elist_init(list_t *h, char *name, u_int type)
{
  list_t	*exist;

  NOPROFILER_IN();
  if (type >= aspect_type_nbr)
    {
      fprintf(stderr, "Unable to initialize list %s \n", name);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to initialize list", -1);
    }
  exist = elist_find(name);
  if (exist)
    {
#if 1 //__LIST_DEBUG__
      fprintf(stderr, "DEBUG: List %s (%p) already exists in hash (%s) with addr %p : NOT CREATING \n", 
	      name, h, h->name, exist);
#endif
      NOPROFILER_ROUT(1);
    }
#if __LIST_DEBUG__
  else
    fprintf(stderr, "DEBUG: List %s allocated at %p does not exists in hash : CREATING \n", name, h);
#endif

  bzero(h, sizeof(list_t));
  h->type   = type;
  h->name   = name;
  hash_add(hash_lists, name, h);
  NOPROFILER_ROUT(0);
}

/** 
 * @brief Return a list by its name 
 */
list_t		*elist_find(char *name)
{
  return ((list_t *) hash_get(hash_lists, name));
}

/** 
 * @brief Set a list by its name (overwrite if existing ) 
 */
int		elist_register(list_t *list, char *name)
{
  list_t	*h;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  h = hash_get(hash_lists, name);
  if (h)
    {
      if (h->type == ASPECT_TYPE_UNKNOW)
	h->type = list->type;
      if (h->type != list->type)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Incompatible lists", -1);
      if (h->elmnbr)
	h = elist_empty(name);
      elist_merge(h, list);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  XALLOC(__FILE__, __FUNCTION__, __LINE__, h, sizeof(list_t), -1);
  elist_init(h, name, h->type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * @brief Empty a list 
 */
list_t		*elist_empty(char *name)
{
  list_t	*list;
  char		*newname;
  char		type;
  u_int		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  list = elist_find(name);
  if (!list)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
  type    = list->type;
  len = strlen(name);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newname, len + 1, NULL);
  strncpy(newname, name, len);
  hash_del(hash_lists, name);
  elist_destroy(list);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, list, sizeof(list_t), NULL);
  elist_init(list, newname, type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, list);
}


/* Reverse a list */
list_t		*elist_reverse(list_t *l)
{
  list_t	*newlist;
  listent_t	*nextent;
  listent_t	*curent;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  hash_del(hash_lists, l->name);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newlist, sizeof(list_t), NULL);
  elist_init(newlist, l->name, l->type);

  /* Copy the list inserting at the beginning all the time */
  for (curent = l->head; curent; curent = nextent)
    {
      elist_add(newlist, curent->key, curent->data);
      nextent = curent->next;
      XFREE(__FILE__, __FUNCTION__, __LINE__, curent);
    }

  XFREE(__FILE__, __FUNCTION__, __LINE__, l);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newlist);  
}


/* Destroy a list */
void		elist_destroy(list_t *h)
{
  char		**keys;
  int		idx;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  //fprintf(stderr, "DEBUG: Destroying list %s at addr %p \n", h->name, h);

  /* We should not destroy the elements as they might be in other hashes */
  keys = elist_get_keys(h, &keynbr);
  for (idx = 0; idx < keynbr; idx++)
    XFREE(__FILE__, __FUNCTION__, __LINE__, keys[idx]);
  if (keys)
    elist_free_keys(keys);
  hash_del(hash_lists, h->name);
  XFREE(__FILE__, __FUNCTION__, __LINE__, h);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Copy a list */
list_t		*elist_copy(list_t *h, u_char datacopy)
{
  list_t	*newlist;
  listent_t	*newent;
  listent_t	*prevent;
  listent_t	*curent;
  void		*newelem;
  int		size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (datacopy != ELIST_DATA_COPY && datacopy != ELIST_DATA_NOCOPY)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid datacopy flag parameter", NULL);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, newlist, sizeof(list_t), NULL);
  *newlist = *h;
  prevent = NULL;
  size = aspect_typesize_get(h->type);

  /* Copy the list */
  for (curent = h->head; curent; curent = curent->next)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, newent, sizeof(listent_t), NULL);
      *newent = *curent;

      // XXX: This is incorrect if linked data structures are stored in list
      if (datacopy == ELIST_DATA_COPY)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, newelem, size, NULL);
	  memcpy(newelem, curent->data, size);
	}
      else
	newelem = curent->data;

      newent->data = newelem;

      newent->key = strdup(curent->key);
      newent->next = NULL;
      if (prevent)
	prevent->next = newent;
      else
	newlist->head = newent;
      prevent = newent;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newlist);
}



/** 
 * @brief Add an element at the head of the list 
 */
int		elist_add(list_t *h, char *key, void *data)
{
  listent_t	*cur;
  listent_t	*next;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !key || !data)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, cur, sizeof(listent_t), -1);
  next = h->head;
  cur->key = key;
  cur->data = data;
  cur->next = next;
  h->head = cur;
  h->elmnbr++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * @brief Add an element at the head of the list 
 */
int		elist_append(list_t *h, char *key, void *data)
{
  listent_t	*cur;
  listent_t	*next;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !key || !data)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", -1);
  if (!h->head)
    {
      ret = elist_add(h, key, data);
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to append list element", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  XALLOC(__FILE__, __FUNCTION__, __LINE__, cur, sizeof(listent_t), -1);
  cur->key = key;
  cur->data = data;
  cur->next = NULL;
  next = h->head;
  while (next->next)
    next = next->next;
  next->next = cur;
  h->elmnbr++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* 
 * @brief Push an element on the list (used as a stack)
 */
int		elist_push(list_t *h, void *data)
{
  char		key[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !data)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", -1);

  snprintf(key, sizeof(key), "%s_%u", h->name, h->elmnbr);
  elist_add(h, strdup(key), data);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* 
 * @brief Pop an element off the list (used as a stack)
 */
void		*elist_pop(list_t *h)
{
  listent_t	*next;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !h->head)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input list", NULL);
  next = h->head;
  h->head = h->head->next;
  h->elmnbr--;
  XFREE(__FILE__, __FUNCTION__, __LINE__, next);
  if (!h->head)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, h->head);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, h->head->data);
}


/** 
 * @brief Delete an element from a list 
 */
int		elist_del(list_t *h, char *key)
{
  listent_t	*curelem;
  listent_t	*prevelem;
  listent_t	*todel;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !key)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", -1);
  curelem = h->head;
  prevelem = NULL;
  for (curelem = h->head; curelem && strcmp(curelem->key, key); 
       curelem = curelem->next)
    prevelem = curelem;
  if (!curelem)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (!prevelem)
    {
      todel = h->head;
      h->head = h->head->next;
    }
  else
    {
      todel = prevelem->next;
      prevelem->next = prevelem->next->next;
    }
  h->elmnbr--;
  if (!h->elmnbr)
    h->head = NULL;
  XFREE(__FILE__, __FUNCTION__, __LINE__, todel);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * @brief Get the list element giving the key 
 */
void		*elist_get(list_t *h, char *key)
{
  listent_t	*cur;
  
  if (!h || !key)
    return (NULL);
  for (cur = h->head; cur; cur = cur->next)
    if (!strcmp(cur->key, key))
      return (cur->data);
  return (NULL);
}

/** 
 * @brief Get the list data giving the key 
 */
void 		*elist_select(list_t *h, char *key)
{
  listent_t	*cur;
  
  if (!h || !key)
    return (NULL);
  for (cur = h->head; cur; cur = cur->next)
    if (!strcmp(cur->key, key))
      return (cur->data);
  return (NULL);
}

/* Get the list head */
listent_t	*elist_get_head(list_t *h)
{
  if (!h)
    return (NULL);
  return (h->head);
}

/* Get the list head data */
void		*elist_get_headptr(list_t *h)
{
  if (!h || !h->head)
    return (NULL);
  return (h->head->data);
}

/* Change the metadata for an existing entry, giving its key */
int		elist_set(list_t *h, char *key, void *data)
{
  listent_t	*cur;
  
  if (!h || !key)
    return (-1);
  for (cur = h->head; cur; cur = cur->next)
    if (!strcmp(cur->key, key))
      {
	cur->data = data;
	return (0);
      }

  printf("ELIST_SET: FAILED to set list (%s) element with key %s \n", h->name, key);
  return (-1);
}

/** 
 * @brief Replace a single element by a list of elements 
 */
int		elist_replace(list_t *h, char *key, list_t *newlist)
{
  listent_t	*cur;
  listent_t	*lastent;
  listent_t	*prev;

  /* Preliminary checks */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !key || !newlist || !newlist->head)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", 0);

  /* Just find the latest entry */
  for (lastent = newlist->head; lastent && lastent->next; lastent = lastent->next);

  /* Now find the original element to replace by the new list */
  for (prev = NULL, cur = h->head; cur; prev = cur, cur = cur->next)
    if (!strcmp(cur->key, key))
      {
	if (!prev)
	  h->head = newlist->head;
	else
	  prev->next = newlist->head;
	lastent->next = cur->next;	
	h->elmnbr += newlist->elmnbr - 1;
	//XFREE(__FILE__, __FUNCTION__, __LINE__, cur->data);
	XFREE(__FILE__, __FUNCTION__, __LINE__, cur);
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      }

  /* Could not find the element to swap */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	       "Cannot find element to be swapped", -1);
}


/* Return the array of keys */
char**		elist_get_keys(list_t *h, int* n)
{
  char		**keys;
  listent_t	*curelem;
  int		idx;

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
  for (idx = 0, curelem = h->head; curelem; curelem = curelem->next, idx++)
    keys[idx] = curelem->key;
  if (n)
    *n = h->elmnbr;
  keys[idx] = NULL;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, keys);
}

/* Free the keys array */
void		elist_free_keys(char **keys)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (keys)
    XFREE(__FILE__, __FUNCTION__, __LINE__, keys);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Get -the list entry- for a given key */
listent_t 	*elist_get_ent(list_t *h, char *key)
{
  listent_t	*cur;
  
  if (!h || !key)
    return (NULL);
  for (cur = h->head; cur; cur = cur->next)
    if (!strcmp(cur->key, key))
      return (cur);
  return (NULL);
}

/* Print list content */
void		elist_print(list_t *h)
{
  listent_t     *actual;
  int           index;

  if (!h)
    return;
  puts(".::. Printing list .::. ");
  for (index = 0, actual = h->head; index < h->elmnbr; index++, actual = actual->next)
    printf(" ENT [%u] key = %s ; data = %p ; next = %p\n",
	   index, actual->key, actual->data, actual->next);
}

/* Apply a function on all elements of the list */
int             elist_apply(list_t *h, void *ptr, 
			   int (*func)(listent_t *e, void *p))
{
  int           index;
  int		ret = 0;
  listent_t	*cur;
  
  if (!h || !func)
    return (-1);
  for (cur = h->head, index = 0; index < h->elmnbr; index++, cur = cur->next)
    ret |= func (cur, ptr);
  return ret;
}

/* Merge two list */
int		elist_merge(list_t *dst, list_t *src)
{
  int           index;
  listent_t	*cur;
  
  if (!dst || !src)
    return (-1);
  for (cur = src->head, index = 0; index < src->elmnbr; index++, cur = cur->next)
    elist_add(dst, cur->key, cur->data);
  return 0;
}

/* Unmerge two lists */
int		elist_unmerge(list_t *dst, list_t *src)
{
  listent_t	*cur;
  
  if (!dst || !src)
    return (-1);
  for (cur = src->head; cur; cur = cur->next)
    elist_del(dst, cur->key);
  return 0;
}

/* Return the size of a list */
int		elist_size(list_t *h)
{
  if (!h)
    return (0);
  return (h->elmnbr);
}

/* Compare two lists */
/* Unimplemented */
int		elist_compare(list_t *first, list_t *two)
{
  return (-1);
}

/* Linear typing of list API */
u_char		elist_linearity_get(list_t *l)
{
  if (!l)
    return (0);
  return (l->linearity);
}

/* Linear typing of list API */
void		elist_linearity_set(list_t *l, u_char val)
{
  if (!l)
    return;
  l->linearity = val;
}
