/*
** vectors.c for libaspect in elfsh
**
** Implement the modularity for the framework
**
** Started Dec 22 2006 02:57:03 mayhem
**
*/
#include "libaspect.h"


/* The hash tables of vectors */
hash_t	       vector_hash;
static u_short vh_set = 0;


/* Initialize the vector hash */
void		aspect_vectors_init()
{
  if (!vh_set)
    {
      hash_init(&vector_hash, 11);
      vh_set = 1;
    }
}

/* Retreive a vector from the hash table giving its name */
vector_t*	aspect_vector_get(char *name)
{
  vector_t	*vect;

  vect = hash_get(&vector_hash, name);
  return (vect);
}

/* Retreive the hash table : useful when iterating over it */
hash_t*		aspect_vecthash_get()
{
  return (&vector_hash);
}


/* Project each dimension and write the desired function pointer */
void		aspect_vectors_insert(vector_t	   *vect, 
				      unsigned int *dim, 
				      unsigned long fct)
{
  unsigned long	*tmp;
  unsigned int	idx;
  unsigned int  dimsz;

  dimsz = vect->arraysz;
  tmp   = vect->hook;
  for (idx = 0; idx < dimsz; idx++)
    {
      tmp += dim[idx];
      if (idx + 1 < dimsz)
	tmp  = (unsigned long *) *tmp;
    }
  *tmp = (unsigned long) fct;
}


/* Project each dimension and get the requested function pointer */
void*			aspect_vectors_select(vector_t *vect, unsigned int *dim)
{
  unsigned long		*tmp;
  unsigned int		idx;
  unsigned int		dimsz;

  tmp   = vect->hook;
  dimsz = vect->arraysz;
  for (idx = 0; idx < dimsz; idx++)
    {
      tmp += dim[idx];
      tmp  = (unsigned long *) *tmp;
    }
  return (tmp);
}


/* Allocate recursively the hook array */
static int	aspect_vectors_recalloc(unsigned long *tab, unsigned int *dims, 
					unsigned int depth, unsigned int dimsz)
{
  unsigned int	idx;

  if (depth == dimsz)
    return (0);
  for (idx = 0; idx < dims[depth - 1]; idx++)
    {
      tab[idx] = (unsigned long) elfsh_calloc(dims[depth] * sizeof(unsigned long), 
					      1);
      if (tab[idx] == (unsigned long) NULL) 
	{
	  write(1, "Out of memory\n", 14);
	  return (-1);
	}
      aspect_vectors_recalloc((unsigned long *) tab[idx], dims, depth + 1, dimsz);
    }
  return (0);
}


/* Initialize recursively the hook array */
static int	aspect_vectors_recinit(unsigned long *tab, unsigned int *dims, 
				       unsigned int depth, unsigned int dimsz,
				       void *defaultelem)
{
  unsigned int	idx;

  /* Check if we reached a leaf, otherwise recurse more */
  if (depth == dimsz)
    {
      for (idx = 0; idx < dims[depth - 1]; idx++)
	tab[idx] = (unsigned long) defaultelem;
    }
  else
    for (idx = 0; idx < dims[depth - 1]; idx++)
      aspect_vectors_recinit((unsigned long *) tab[idx], dims, 
			     depth + 1, dimsz, defaultelem);
  return (0);
}



/* Register a new vector. A vector is an multidimentional array of hooks */
int		aspect_register_vector(char		*name, 
				      void		*registerfunc, 
				      void		*defaultfunc,
				      unsigned int	*dimensions, 
				      unsigned int	dimsz)
{
  vector_t	*vector;
  unsigned long	*ptr;
  
  if (!registerfunc || !defaultfunc || !dimsz || !dimensions)
    {
      write(1, "Invalid NULL parameters\n", 24);
      return (-1);
    }
  vector = calloc(sizeof(vector_t), 1);
  if (vector == NULL)
    return (-1);
  ptr = calloc(dimensions[0] * sizeof(unsigned long), 1);
  if (!ptr)
    return (-1);
  vector->hook = ptr;
  if (dimsz > 1)
    aspect_vectors_recalloc((unsigned long *) vector->hook, dimensions, 1, dimsz);
  
  vector->arraysz       = dimsz;
  vector->arraydims     = dimensions;
  vector->register_func = registerfunc;
  vector->default_func  = defaultfunc;
  hash_add(&vector_hash, name, vector);

  /* Initialize vectored elements */
  aspect_vectors_recinit(vector->hook, dimensions, 1, dimsz, defaultfunc);
  return (0);
}




/* Display the vector recursively */
void	aspect_vector_recdisplay(unsigned long *tab,  unsigned int *dims, 
				 unsigned int *index, unsigned int depth, 
				 unsigned int dimsz, char *name)
{
  unsigned int	idx;
  unsigned int	idx2;  

  /* Check if we reached the last dimension */
  if (depth == dimsz)
    for (idx = 0; idx < dims[depth - 1]; idx++)
      {
	printf("   %s", name);
	index[depth - 1] = idx;
	for (idx2 = 0; idx2 < depth; idx2++)
	  printf("[%02u]", index[idx2]);
	printf(" = $0x%08lX \n", tab[idx]);
      }

  /* Otherwise recurse more */
  else
    for (idx = 0; idx < dims[depth - 1]; idx++)
      {
	index[depth - 1] = idx;
	aspect_vector_recdisplay((unsigned long *) tab[idx], dims, index,
				 depth + 1, dimsz, name);
      }
  return;  
}


/* Display vector */
void		aspect_vector_display(vector_t *cur, char *name)
{
  unsigned int	*idx;

  idx = alloca(cur->arraysz * sizeof(unsigned int));
  bzero(idx, cur->arraysz * sizeof(unsigned int));
  printf("  .:: Printing vector %s \n\n", name);
  aspect_vector_recdisplay(cur->hook, cur->arraydims, idx, 
			   1, cur->arraysz, name);
  printf("\n .:: Vector %s printed \n\n", name);
}



/* Display the content of a vector */
void		aspect_vectors_display()
{
  char		**keys;
  int		keynbr;
  int		index;
  int		index2;
  vector_t	*cur;

  printf("  .:: Registered vectors \n\n");
  keys = hash_get_keys(&vector_hash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      cur = hash_get(&vector_hash, keys[index]);
      printf("  + %-30s\t Dimensions: ", keys[index]);
      for (index2 = 0; index2 < cur->arraysz; index2++)
	printf("[%02u]", cur->arraydims[index2]);
      printf("\n");
    }
  printf("\n Type vector vectname for specific vector details.\n\n");
}
