/*
** libaspect-list.h for libaspect in ERESI
**
** Prototypes of API for ERESI lists
**
** Started on Fri Jul 13 20:25:42 2007 mayhem
** $Id: libaspect-list.h,v 1.2 2007-07-19 02:41:26 may Exp $
*/

#ifndef _LIBLIST_H_
 #define _LIBLIST_H_ 1

/* List entry data structure */
typedef struct          s_listent
{
  char                  *key;
  void                  *data;
  struct s_listent      *next;
}                       listent_t;

/* List data structure */
typedef struct          s_aspect_list
{
  listent_t             *head;
  int			elmnbr;
  u_int			type;
  u_char		linearity;
}                       list_t;


/* The hash table of list is accessible to the public */
extern hash_t		*hash_lists;


/* list.c */
int		list_init(list_t *, char*, u_int);	/* Allocate the list */
list_t		*list_find(char *name);			/* Find a list */
int		list_register(list_t *h, char *name);	/* Register a list */
list_t		*list_empty(char *name);		/* Empty the list */
void		list_destroy(list_t *h);		/* Free the list */
int		list_add(list_t *h, char *k, void *d);	/* Add an entry */
int		list_del(list_t *h, char *key);		/* Delete an entry */
void		*list_get(list_t *h, char *key);	/* Get data from key */
void 		*list_select(list_t *h, char *key);	/* Get an entry pointer */
listent_t	*list_get_head(list_t *h);		/* Get a list head */
listent_t 	*list_get_ent(list_t *h, char *key);	/* Get an entry metadata */
void		list_print(list_t *h);			/* Print the list */
char**		list_get_keys(list_t *h, int* n);	/* Create array of keys */
void		list_free_keys(char **keys);		/* Free keys */
int             list_apply(list_t *h, void *ptr, 
			   int (*f)(listent_t *e, void *p)); /* Apply function */
int		list_merge(list_t *dst, list_t *src);	/* Fuse lists */
int		list_unmerge(list_t *dst, list_t *src); /* Quotient lists */
int		list_size(list_t *hash);		/* Return the elm nbr */
int		list_set(list_t *h, char *key, void *data); /* Change meta data for a key */
int		list_replace(list_t *h, char *k, list_t *nl); /* Replace one elem by a list */
int		list_compare(list_t *first, list_t *two);     /* Compare the content of 2 lists */
u_char		list_linearity_get(list_t *l);		      /* Get linearity of a list */
void		list_linearity_set(list_t *l, u_char val);    /* Set linearity of a list */

#endif /* _LIBLIST_H_ */
