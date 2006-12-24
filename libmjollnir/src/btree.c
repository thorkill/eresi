/* 
** libbtree.c in libmjollnir for elfsh
** 
** Author  : sk at devhell dot org
**
** Started : Fri Oct 17 14:29:24 2003
** Updated : Thu Nov 27 23:29:29 2003
*/
#include "libmjollnir.h"


/* Insert element in tree using id to sort it */
void	btree_insert(btree_t **proot,		/* ptr to btree root	*/
		     u_int id,			/* element id		*/
		     void *elem)		/* ptr to element	*/
{
  btree_t	*root;
  
  root = *proot;
  if (!root)
    {
      root = elfsh_malloc(sizeof (btree_t));
      memset(root, 0, sizeof (btree_t));
      root->id = id;
      root->elem = elem;
      *proot = root;
    }
  else
    {
      if (id < root->id)
	btree_insert(&root->left, id, elem);
      else
	btree_insert(&root->right, id, elem);
    }
}



/**
 * insert element in tree using a function to sort it.
 * depending on function return value, element is insert
 * in left or right path:
 * if return value is null, element is already present in
 * tree and is discarded.
 *
 * if apply function return a positive value,
 * element is inserted on right path
 * if apply function return a negative value,
 * element is inserted on left path
 */
void	btree_insert_sort(btree_t **proot,		/* ptr to root	*/
			  int (*apply)(void *, void *), /* cmp handler	*/
			  void *elem)			/* element	*/
{
  btree_t	*root;
  int		ret;
  root = *proot;
  
  if (!root)
    btree_insert(proot, (u_int) elem, elem);
  else
    {
      if ((ret = apply(root->elem, elem)))
	{
	  if (ret > 0)
	    btree_insert_sort(&root->right, apply, elem);
	  else
	    btree_insert_sort(&root->left, apply, elem);
	}	
    }  
}    

/**
 * return an element by providing its id.
 * id is id provided to insert element
 * with btree_insert();
 * do not use on trees built with btree_insert_sort
 */

void	*btree_get_elem(btree_t *root,	/* ptr to root		*/
			u_int id)	/* element id to fetch	*/
{
  if (!root)
    return (NULL);
  /* elem found, return it */
  if (root->id == id)
    return (root->elem);
  /* switch on path */
  if (root->id < id)
    return (btree_get_elem(root->left, id));
  else if (root->id > id)
    return (btree_get_elem(root->right, id));
  return (NULL);
}



/**
 * return an element by providing a matching function
 * this matching function is used to follow path
 * until it find element depending on its return value:
 * if match function is null, element is current element
 * and is returned.
 * else
 * if return value is negative, element is searched in right path
 * if return value is positive, element is searched in left path
 *
 */
void		*btree_find_elem(btree_t *root, 
				 int	 (*match)(void *, void *), 
				 void	 *ptr)
{
  int		ret;
  
  if (!root)
    return (NULL);
  ret = match(root->elem, ptr);
  if (!ret)
    return (ptr);
  else if (ret > 0)
    return (btree_find_elem(root->right, match, ptr));
  else
    return (btree_find_elem(root->left, match, ptr));
}


/*
 * browse tree and call function apply on each element.
 * ptr is a pointer passed as second argument of apply
 */
void	btree_browse_prefix(btree_t *root, int (*apply)(void *, void *), void *ptr)
{
  if (root)
    {
      if (apply(root->elem, ptr) != 0)
	{
	  fprintf(stderr, "apply handler returned != 0, backtracking\n");
	  fflush(stdout);
	  return;
	}
      else
	{
	  fprintf(stderr, "apply returned 0 \n");
	  fflush(stdout);
	}

      if (root->left)
	{
	  printf("Going on the left of %08X \n", 
		 ((elfshiblock_t *) root->elem)->vaddr);
	  btree_browse_prefix(root->left, apply, ptr);
	  printf("Back from the left of %08X \n", 
		 ((elfshiblock_t *) root->elem)->vaddr);
	}
      if (root->right)
	{
	  printf("Going on the right of %08X \n", 
		 ((elfshiblock_t *) root->elem)->vaddr);
	  btree_browse_prefix(root->right, apply, ptr);
	  printf("Going on the right of %08X \n", 
		 ((elfshiblock_t *) root->elem)->vaddr);
	}
    }
}



void	btree_browse_infix(btree_t *root, int (*apply)(void *, void *), void *ptr)
{
  if (root)
    {
      if (root->left)
	btree_browse_infix(root->left, apply, ptr);
      apply(root->elem, ptr);
      if (root->right)
	btree_browse_infix(root->right, apply, ptr);
    }
}

void	btree_browse_suffix(btree_t *root, int (*apply)(void *, void *), void *ptr)
{
  if (root)
    {
      if (root->left)
	btree_browse_suffix(root->left, apply, ptr);
      if (root->right)
	btree_browse_suffix(root->right, apply, ptr);
      apply(root->elem, ptr);
    }
}

/**
 * free full try.
 * if mode is not null, elem is free-ed too
 */

void	btree_free(btree_t *root, int mode)
{
  printf("Calling free on block %08X \n", ((elfshiblock_t *)root->elem)->vaddr);
  if (root)
    {
      if (mode)
	XFREE(root->elem);
      btree_free(root->left, mode);
      btree_free(root->right, mode);
      XFREE(root);
    }
}

/**
 *
 *
 */
int	btree_debug_node(void *elem, void *ptr, btree_t *root)
{
  struct s_debug	*opt;

  opt = (struct s_debug *) ptr;
  
  fprintf(opt->fp, BTREE_DEBUG_NODE, (int) root, (int) root, 
	  (int) root->left, (int) root->right);
  return (0);
}

int	btree_debug_link(void *elem, void *ptr, btree_t *root)
{
  struct s_debug	*opt;
  
  opt = (struct s_debug *) ptr;
  
  if (root->left)
    fprintf(opt->fp, BTREE_DEBUG_LINK, (int) root, 
	    "L", (int) root->left, opt->link++);
  if (root->right)
    fprintf(opt->fp, BTREE_DEBUG_LINK, (int) root, "R", 
	    (int) root->right, opt->link++);
  return (0);
}

void	btree_browse_prefix_debug(btree_t *root, int (*apply)(void *, void *, btree_t *), 
				  void *ptr)
{
  if (root)
    {
      apply(root->elem, ptr, root);
      if (root->left)
	btree_browse_prefix_debug(root->left, apply, ptr);
      if (root->right)
	btree_browse_prefix_debug(root->right, apply, ptr);
    }
}


void	btree_debug(btree_t *root, char *filename, 
		    void (*apply)(void *, void *)) {
  struct s_debug opt;
  opt.fp = fopen(filename, "w");
  if (opt.fp)
    {
      fprintf(opt.fp, "digraph g {\n"
	      "size=\"6,4\"; ratio = fill; graph [ rankdir = \"LR\" ] ;\n");
      btree_browse_prefix_debug(root, btree_debug_node, &opt);
      opt.link = 0;
      btree_browse_prefix_debug(root, btree_debug_link, &opt);
      fprintf(opt.fp, "};\n");
      fclose(opt.fp);
    }
}

