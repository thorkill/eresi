/*
** @file vectors.c
**
** @brief Implement the modularity for the framework
**
** Started Dec 22 2006 02:57:03 jfv
**
**
** $Id: vectors.c,v 1.24 2007-11-28 07:56:08 may Exp $
**
*/
#include "libaspect.h"

hash_t	       *vector_hash = NULL;


/**
 * @brief Retreive a vector from the hash table giving its name 
 *
 * @param name
 * @return 
 */
vector_t*	aspect_vector_get(char *name)
{
  vector_t	*vect;

  if (!vector_hash)
    {
      printf("Tried to get a vector when hash table = NULL \n");
      sleep(30);
      return (NULL);
    }

  vect = (vector_t *) hash_get(vector_hash, name);
  return (vect);
}

/** 
 * @brief Retreive the hash table : useful when iterating over it 
 *
 * @return Return a pointer to the global hash table
 */
hash_t*		aspect_vecthash_get()
{
  return (vector_hash);
}


/** 
 * @brief Project each dimension and write the desired function pointer 
 *
 * @param vect
 * @param dim
 * @param fct
 */
void		aspect_vectors_insert(vector_t	   *vect, 
				      unsigned int *dim, 
				      unsigned long fct)
{
  unsigned long	*tmp;
  unsigned int	idx;
  unsigned int  dimsz;

  dimsz = vect->arraysz;
  tmp   = (unsigned long *) vect->hook;
  for (idx = 0; idx < dimsz; idx++)
    {
      tmp += dim[idx];
      if (idx + 1 < dimsz)
	tmp  = (unsigned long *) *tmp;
    }
  *tmp = (unsigned long) fct;
}


/**
 * @brief Project each dimension and get the requested function pointer 
 *
 * @param vect
 * @param dim
 * @return Return a function pointer
 */
void*			aspect_vectors_select(vector_t *vect, unsigned int *dim)
{
  unsigned long		*tmp;
  unsigned int		idx;
  unsigned int		dimsz;

  tmp   = (unsigned long *) vect->hook;
  dimsz = vect->arraysz;
  for (idx = 0; idx < dimsz; idx++)
    {
      tmp += dim[idx];
      tmp  = (unsigned long *) *tmp;
    }
  return (tmp);
}


/** 
 * @brief Project each dimension and get the requested data pointer 
 *
 * @param vect
 * @param dim
 * @return
 */
void		*aspect_vectors_selectptr(vector_t * vect, 
					  unsigned int *dim)
{
  unsigned long *tmp;
  unsigned int	idx;
  unsigned int	dimsz;

  tmp = (unsigned long *) vect->hook;
  dimsz = vect->arraysz;
  for (idx = 0; idx < dimsz; idx++)
    {
      tmp += dim[idx];
      if (idx + 1 < dimsz)
        tmp = (unsigned long *) *tmp;
    }
  return (tmp);
}




/** 
 * @brief Allocate recursively the hook array
 *
 * @param tab
 * @param dims
 * @param depth
 * @param dimsz
 * return 
 */
static int	aspect_vectors_recalloc(unsigned long *tab, 
					unsigned int *dims, 
					unsigned int depth, 
					unsigned int dimsz)
{
  unsigned int	idx;
  void		*ptr;

  //PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //printf("Calling recalloc with depth = %u and dimsz = %u\n", 
  //depth, dimsz);

  if (depth == dimsz)
    return (0);
    //PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  for (idx = 0; idx < dims[depth - 1]; idx++)
    {

      //XALLOC(__FILE__, __FUNCTION__, __LINE__,
      //ptr, dims[depth] * sizeof(unsigned long), -1);
      
      ptr = calloc(dims[depth] * sizeof(unsigned long), 1);
      if (!ptr)
	return (-1);

      tab[idx] = (unsigned long) ptr;
      aspect_vectors_recalloc((unsigned long *) tab[idx], 
			      dims, depth + 1, dimsz);
    }


  //printf("GETTING OUT OF recalloc with depth = %u and dimentnbr = %u\n", 
  // depth, dimsz);

  return (0);
  //PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * @brief Initialize recursively the hook array 
 */
static int	aspect_vectors_recinit(unsigned long *tab, 
				       unsigned int *dims, 
				       unsigned int depth, 
				       unsigned int dimsz,
				       void *defaultelem)
{
  unsigned int	idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * @brief Register a new vector. A vector is an multidimentional array of hooks
 *
 * @param name
 * @param defaultfunc
 * @param dimensions 
 * @param strdims
 * @param dimsz
 * @param vectype
 *
 */
int		aspect_register_vector(char		*name, 
				       void		*defaultfunc,
				       unsigned int	*dimensions, 
				       char		**strdims,
				       unsigned int	dimsz,
				       unsigned int	vectype)
{
  vector_t	*vector;
  unsigned long	*ptr;
  void		*mem;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!defaultfunc || !dimsz || !dimensions)
    {
      write(1, "Invalid NULL parameters\n", 24);
      return (-1);
    }
  if (vectype >= aspect_type_nbr)
    {
      write(1, "Invalid vector element type\n", 28);
      return (-1);
    }

  XALLOC(__FILE__, __FUNCTION__, __LINE__, mem, sizeof(vector_t), -1);
  vector = (vector_t *) mem;

  XALLOC(__FILE__, __FUNCTION__, __LINE__, mem, 
	 dimensions[0] * sizeof(unsigned long), -1);
  ptr = (unsigned long *) mem;

  vector->hook = ptr;
  if (dimsz > 1)
    aspect_vectors_recalloc((unsigned long *) vector->hook, 
			    dimensions, 1, dimsz);
  
  vector->arraysz       = dimsz;
  vector->arraydims     = dimensions;
  vector->strdims       = strdims;
  vector->default_func  = defaultfunc;
  hash_add(vector_hash, name, vector);

  /* Initialize vectored elements */
  aspect_vectors_recinit((unsigned long *) vector->hook, 
			 dimensions, 1, dimsz, defaultfunc);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}
