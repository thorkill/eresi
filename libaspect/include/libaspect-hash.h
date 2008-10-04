/*
** @file libhash.h
**
** Started on  Fri Jan 24 20:25:42 2003 jfv
** Last update Fri Dec 22 00:14:56 2006 jfv
**
** $Id: libaspect-hash.h,v 1.10 2007-11-28 07:56:08 may Exp $
**
*/
#ifndef _LIBHASH_H_
 #define _LIBHASH_H_ 1

#ifdef __BEOS__
 #include <bsd_mem.h>
#endif

#include "libaspect-list.h"

/**
 * @brief Hash table
 */
typedef struct          s_hash
{
  listent_t             *ent;
  int                   size;
  int			elmnbr;
  unsigned int			type;
  unsigned char		linearity;
  char			*name;
}                       hash_t;

/**
 * @brief  The hash table of hash tables is accessible to the public 
 */
extern hash_t		*hash_hash;

/** 
 * @brief The hash table of list is accessible to the public 
 */
extern hash_t		*hash_lists;

/* hash.c */
int		hash_init(hash_t*, char*, int, unsigned int);  /* Allocate the table */
hash_t		*hash_find(char *name);			/* Find a hash table */
int		hash_register(hash_t *h, char *name);	/* Register a hash table */
hash_t		*hash_empty(char *name);		/* Empty the hash table */
void		hash_destroy(hash_t *h);		/* Free the table */
int		hash_add(hash_t*, char*, void *);       /* Add an entry */
int		hash_del(hash_t *h, char *key);		/* Delete an entry */
void		*hash_get(hash_t *h, char *key);	/* Get data from key */
void 		*hash_select(hash_t *h, char *key);	/* Get an entry pointer */
listent_t	*hash_get_head(hash_t *h, char *b);	/* Get a list head */
listent_t 	*hash_get_ent(hash_t *h, char *key);	/* Get an entry metadata */
void		hash_print(hash_t *h);			/* Print the hash table */
char**		hash_get_keys(hash_t *h, int* n);	/* Create array of keys */
void		hash_free_keys(char **keys);		/* Free keys */
int             hash_apply(hash_t *h, void *ptr, 
			   int (*f)(listent_t *e, void *p)); /* Apply function */
int		hash_merge(hash_t *dst, hash_t *src);	/* Fuse hashes */
int		hash_unmerge(hash_t *dst, hash_t *src); /* Quotient hashes */
int		hash_size(hash_t *hash);		/* Return the elm nbr */
void*		hash_get_one(hash_t *hash);		/* Get any object */
void*		hash_get_single(hash_t *hash);		/* Get _the_ only object */
int		hash_set(hash_t *h, char *key, void *data); /* Change meta data for a key */
unsigned char		hash_linearity_get(hash_t *h);		/* Get hash table linearity */
void		hash_linearity_set(hash_t *h, unsigned char v); /* Set hash table linearity */

#endif /* _LIBHASH_H_ */
