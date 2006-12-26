/*
** libhash.h for libaspect from elfsh
**
** Started on  Fri Jan 24 20:25:42 2003 mayhem
** Last update Fri Dec 22 00:14:56 2006 mayhem
*/

#ifndef _LIBHASH_H_
 #define _LIBHASH_H_ 1

#ifdef __BEOS__
 #include <bsd_mem.h>
#endif

/* Safe calloc() */
#define		HASHALLOC(a, b, c)			\
do							\
{							\
  if ((a = calloc(b, 1)) == NULL)			\
    {							\
      perror("libhashelfsh[calloc]");			\
      return (c);					\
    }							\
}							\
while (0)

/*
** Hash table entry
*/
typedef struct          s_hashent
{
  char                  *key;
  void                  *data;
  struct s_hashent      *next;
}                       hashent_t;

/*
** Hash table
*/
typedef struct          s_hash
{
  hashent_t             *ent;
  int                   size;
  int			elmnbr;
}                       hash_t;

/* hash.c */
int		hash_init(hash_t *h, int size);		/* Allocate the table */
void		hash_destroy(hash_t *h);		/* Free the table */
int		hash_add(hash_t *h, char *k, void *d);	/* Add an entry */
int		hash_del(hash_t *h, char *key);		/* Delete an entry */
void		*hash_get(hash_t *h, char *key);	/* Get an entry pointer */
hashent_t	*hash_get_head(hash_t *h, char *b);	/* Get a list head */
hashent_t 	*hash_get_ent(hash_t *h, char *key);	/* Get an entry metadata */
void		hash_print(hash_t *h);			/* Print the hash table */
char**		hash_get_keys(hash_t *h, int* n);	/* XXX: need comment */
int             hash_apply(hash_t *h, void *ptr, 
			   int (*f)(hashent_t *e, void *p)); /* Apply function */
int		hash_merge(hash_t *dst, hash_t *src);	/* Fuse hashes */
int		hash_unmerge(hash_t *dst, hash_t *src); /* Quotient hashes */
int		hash_size(hash_t *hash);		/* Return the elm nbr */
void*		hash_get_one(hash_t *hash);		/* Get any object */
void*		hash_get_single(hash_t *hash);		/* Get _the_ only object */

#endif /* _LIBHASH_H_ */
