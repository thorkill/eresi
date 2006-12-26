/*
** libaspect.h for libaspect in elfsh
**
** The header file for modular objects in the framework
**
** Started Dec 22 2006 02:57:03 mayhem
**
*/
#if !defined(__ASPECT_H__)
 #define __ASPECT_H__ 1

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>

/* When we link inside the elfsh project, dont forget to redirect allocations */
#if defined(ASPECT_ELFSH_INTERN)
void           *elfsh_malloc(size_t);
void           *elfsh_calloc(size_t, char);
void           *elfsh_realloc(void *, size_t);
void		elfsh_free(void *tofree);

#define		malloc  elfsh_malloc
#define		calloc  elfsh_calloc
#define		realloc elfsh_realloc
#define		free	elfsh_free

#endif

/* Include this here since it contains an allocation too */
#include "libhash.h"

/* The structure of multidimensional vectors */
typedef struct	s_vector
{
  void		*hook;
  void		*register_func;
  void		*default_func;
  u_int		*arraydims;
  u_int		arraysz;
}		vector_t;

/* Initialize the vectors of libaspect */
void		aspect_vectors_init();

/* Add the function pointer to the hook at requested coordonates */
void		aspect_project_vectdim(vector_t *vect, unsigned int *dim, 
				       unsigned int dimsz, unsigned long fct);

/* Get the hook from the dimension array */
void*		aspect_project_coords(vector_t *vect, unsigned int *dim, 
				      unsigned int dimsz);

/* Allocate matrixes of hooks */
int		aspect_recursive_vectalloc(unsigned long *tab, unsigned int *dims, 
					   unsigned int depth, unsigned int dimsz);

/* Add a new vector */
int		aspect_register_vector(char *, void*, void*,
				       unsigned int *, unsigned int);

#endif
