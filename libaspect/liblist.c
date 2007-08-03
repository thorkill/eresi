/**
** @file liblist.c
** 
** Contain ELFsh internal lists related API
**
** Started on  Fri Jul 13 20:26:18 2007 jfv
** $Id: liblist.c,v 1.5 2007-08-03 11:50:59 heroine Exp $
*/
#include "libaspect.h"

/* Hash tables of hash tables */
hash_t  *hash_lists = NULL;

/**
 * @brief Initialize the hash table 
 */
int list_init(list_t *h, char *name, u_int type)
{
  NOPROFILER_IN();
  if (type >= aspect_type_nbr)
    {
      fprintf(stderr, "Unable to initialize list %s \n", name);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to initialize list", -1);
    }
  if (list_find(name))
    {
#if __DEBUG__
      fprint(stderr, "List already exists\n");
#endif
      NOPROFILER_ROUT(1);
    }
  
  h->head   = NULL;
  h->type   = type;
  h->elmnbr = 0;

  hash_add(hash_lists, name, h);
  NOPROFILER_ROUT(0);
}

/** 
 * @brief Return a list by its name 
 */
list_t		*list_find(char *name)
{
  return ((list_t *) hash_get(hash_lists, name));
}

/** 
 * @brief Set a list by its name (overwrite if existing ) 
 */
int		list_register(list_t *list, char *name)
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
	h = list_empty(name);
      list_merge(h, list);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }  
  XALLOC(__FILE__, __FUNCTION__, __LINE__, h, sizeof(list_t), -1);
  list_init(h, name, h->type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * @brief Empty a list 
 */
list_t		*list_empty(char *name)
{
  list_t	*list;
  char		*newname;
  char		type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  list = list_find(name);
  if (!list)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
  type    = list->type;
  hash_del(hash_lists, name);
  list_destroy(list);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 newname, strlen(name) + 1, NULL);
  strcpy(newname, name);
  list_init(list, newname, type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, list);
}

/* Destroy a list */
void		list_destroy(list_t *h)
{
  char		**keys;
  int		idx;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We should not destroy the elements as they might be in other hashes */
  keys = list_get_keys(h, &keynbr);
  for (idx = 0; idx < keynbr; idx++)
    XFREE(__FILE__, __FUNCTION__, __LINE__, keys[idx]);
  list_free_keys(keys);
  XFREE(__FILE__, __FUNCTION__, __LINE__, h);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/** 
 * @brief Add an element at the head of the list 
 */
int		list_add(list_t *h, char *key, void *data)
{
  listent_t	*cur;
  listent_t	*next;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !key || !data)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 cur, sizeof(listent_t), -1);
  next = h->head;
  cur->key = key;
  cur->data = data;
  cur->next = next;
  h->head = cur;
  h->elmnbr++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * @brief Delete an element from a list 
 */
int		list_del(list_t *h, char *key)
{
  listent_t	*curelem;
  listent_t	*prevelem;
  
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
    h->head = h->head->next;
  else
    prevelem->next = prevelem->next->next;
  h->elmnbr--;
  if (!h->elmnbr)
    h->head = NULL;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * @brief Get the list element giving the key 
 */
void		*list_get(list_t *h, char *key)
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
void 		*list_select(list_t *h, char *key)
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
listent_t	*list_get_head(list_t *h)
{
  if (!h)
    return (NULL);
  return (h->head);
}

/* Change the metadata for an existing entry, giving its key */
int		list_set(list_t *h, char *key, void *data)
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
  return (-1);
}

/** 
 * @brief Replace a single element by a list of elements 
 */
int		list_replace(list_t *h, char *key, list_t *newlist)
{
  listent_t	*cur;
  listent_t	*lastent;
  listent_t	*prev;

  if (!h || !key || !newlist)
    return (-1);
  for (lastent = newlist->head; lastent && lastent->next; lastent = lastent->next);
  if (!lastent)
    return (-1);
  for (prev = NULL, cur = h->head; cur; prev = cur, cur = cur->next)
    if (!strcmp(cur->key, key))
      {
	if (!prev)
	  {
	    lastent->next = h->head;
	    h->head = newlist->head;
	    h->elmnbr++;
	    return (0);
	  }
	lastent->next = cur->next;
	prev->next = newlist->head;
	h->elmnbr += newlist->elmnbr;
	return (0);
      }
  return (-1);
}


/* Return the array of keys */
char**		list_get_keys(list_t *h, int* n)
{
  char		**keys;
  listent_t	*curelem;
  int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, keys, 
	 sizeof(char *) * h->elmnbr, NULL);
  for (idx = 0, curelem = h->head; curelem; curelem = curelem->next, idx++)
    keys[idx] = curelem->key;
  if (n)
    *n = h->elmnbr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, keys);
}

/* Free the keys array */
void		list_free_keys(char **keys)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XFREE(__FILE__, __FUNCTION__, __LINE__, keys);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Get -the list entry- for a given key */
listent_t 	*list_get_ent(list_t *h, char *key)
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
void		list_print(list_t *h)
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
int             list_apply(list_t *h, void *ptr, 
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
int		list_merge(list_t *dst, list_t *src)
{
  int           index;
  listent_t	*cur;
  
  if (!dst || !src)
    return (-1);
  for (cur = src->head, index = 0; index < src->elmnbr; index++, cur = cur->next)
    list_add(dst, cur->key, cur->data);
  return 0;
}

/* Unmerge two lists */
int		list_unmerge(list_t *dst, list_t *src)
{
  listent_t	*cur;
  
  if (!dst || !src)
    return (-1);
  for (cur = src->head; cur; cur = cur->next)
    list_del(dst, cur->key);
  return 0;
}

/* Return the size of a list */
int		list_size(list_t *h)
{
  if (!h)
    return (0);
  return (h->elmnbr);
}

/* Compare two lists */
/* Unimplemented */
int		list_compare(list_t *first, list_t *two)
{
  return (-1);
}

/* Linear typing of list API */
u_char		list_linearity_get(list_t *l)
{
  if (!l)
    return (0);
  return (l->linearity);
}

/* Linear typing of list API */
void		list_linearity_set(list_t *l, u_char val)
{
  if (!l)
    return;
  l->linearity = val;
}
