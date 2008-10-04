/*
** 
** @file libbtree.h
** 
** Author  : <IDSAFQREOFASOCASDCAADS>
** Started : Fri Oct 17 14:30:27 2003
** Updated : Thu Nov 27 23:27:49 2003
**
** $Id: libaspect-btree.h,v 1.4 2008-02-16 12:32:27 thor Exp $
**
*/

#ifndef __KERNEL__
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
#endif

/**
 * @brief Graphviz node pattern for debugging purpose
 */
#define BTREE_DEBUG_NODE	"\"btree_%08x\" [\n"\
"label = \"<ptr> %8x| <L> %8x|<R> %8x\n"\
"shape = \"record\"\n"\
"];\n"

/**
 * @brief Graphviz link pattern for debugging purpose
 */
#define BTREE_DEBUG_LINK  "\"btree_%08x\":%s -> \"btree_%08x\":ptr [\n"\
"id = %i\n"\
"];\n"

#ifndef __KERNEL__
/**
 *
 */
struct s_debug 
{
  FILE	*fp;
  int	link;
};
#endif

/**
 * @brief Binary tree structure.
 */
typedef struct s_btree
{
  unsigned int		id;	/*!< Id of the node		*/
  void			*elem;	/*!< Pointer to element of the node */
  struct s_btree	*left;	/*!< Left link of the tree	*/
  struct s_btree	*right;	/*!< Right link of the tree	*/
} btree_t;

#define BTREE_FREE_ELEM	1
#define BTREE_FREE_TREE	0

void	btree_insert(btree_t **, unsigned int, void *);
void	btree_insert_sort(btree_t **, int (*)(void *, void *), void *);
void	*btree_get_elem(btree_t *, unsigned int);
void	*btree_find_elem(btree_t *, int (*)(void *, void *), void *);
void	btree_browse_prefix(btree_t *, int (*)(void *, void *), void *);
void	btree_browse_infix(btree_t *, int (*)(void *, void *), void *);
void	btree_browse_suffix(btree_t *, int (*)(void *, void *), void *);
void	btree_free(btree_t *, int);

void	btree_browse_prefix_debug(btree_t *, int (*)(void *, void *, btree_t *), void *);
void	btree_debug(btree_t *, char *, void (*)(void *, void *));
