/*
** @file libaspect-list.h
**
** @brief Prototypes of API for ERESI lists
**
** Started on Fri Jul 13 20:25:42 2007 jfv
** $Id: libaspect-list.h,v 1.8 2008-02-16 12:32:27 thor Exp $
*/

#ifndef _LIBLIST_H_
 #define _LIBLIST_H_ 1

/** 
 * @brief List entry data structure 
 */
typedef struct          s_listent
{
  char                  *key;
  void                  *data;
  struct s_listent      *next;
}                       listent_t;

/** 
 * @brief List data structure 
 */
typedef struct          s_aspect_list
{
  listent_t             *head;
  int			elmnbr;
  unsigned int			type;
  unsigned char		linearity;
  char			*name;
}                       list_t;

/* list.c */
int		elist_init(list_t *, char*, unsigned int);	/* Allocate the list */
list_t		*elist_find(char *name);			/* Find a list */
int		elist_register(list_t *h, char *name);	/* Register a list */
list_t		*elist_empty(char *name);		/* Empty the list */
list_t		*elist_reverse(list_t *l);		/* Reverse the list */
void		elist_destroy(list_t *h);		/* Free the list */
list_t		*elist_copy(list_t *h);			/* Copy a list */
int		elist_add(list_t *h, char *k, void *d);	/* Add an entry */
int		elist_del(list_t *h, char *key);		/* Delete an entry */
void		*elist_get(list_t *h, char *key);	/* Get data from key */
void 		*elist_select(list_t *h, char *key);	/* Get an entry pointer */
listent_t	*elist_get_head(list_t *h);		/* Get a list head */
listent_t 	*elist_get_ent(list_t *h, char *key);	/* Get an entry metadata */
void		elist_print(list_t *h);			/* Print the list */
char**		elist_get_keys(list_t *h, int* n);	/* Create array of keys */
void		elist_free_keys(char **keys);		/* Free keys */
int             elist_apply(list_t *h, void *ptr, 
			   int (*f)(listent_t *e, void *p)); /* Apply function */
int		elist_merge(list_t *dst, list_t *src);	/* Fuse lists */
int		elist_unmerge(list_t *dst, list_t *src); /* Quotient lists */
int		elist_size(list_t *hash);		/* Return the elm nbr */
int		elist_set(list_t *h, char *key, void *data); /* Change meta data for a key */
int		elist_replace(list_t *h, char *k, list_t *nl); /* Replace one elem by a list */
int		elist_compare(list_t *first, list_t *two);     /* Compare the content of 2 lists */
unsigned char		elist_linearity_get(list_t *l);		      /* Get linearity of a list */
void		elist_linearity_set(list_t *l, unsigned char val);    /* Set linearity of a list */
void		*elist_pop(list_t *h);				/* Remove the head of the list */
int		elist_push(list_t *h, void *data);	/* Add an element in first of the list */
void		*elist_get_headptr(list_t *h);		/* Get list head data without popping */

#endif /* _LIBLIST_H_ */
