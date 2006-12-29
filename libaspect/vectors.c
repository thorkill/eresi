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
hash_t	vector_hash;
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

  if (depth == dimsz)
    return (0);

  /* Check if we reached a leaf, otherwise recurse more */
  if (depth == dimsz)
    for (idx = 0; idx < dims[depth - 1]; idx++)
      tab[idx] = (unsigned long) defaultelem;
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
