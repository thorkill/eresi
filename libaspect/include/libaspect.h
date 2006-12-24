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

#include "libhash.h"
#include <unistd.h>

/* The structure of multidimensional vectors */
typedef struct	s_elfshvector
{
  void		*hook;
  void		*register_func;
  void		*default_func;
  u_int		*arraydims;
  u_int		arraysz;
}		elfshvector_t;

void           *elfsh_malloc(size_t);
void           *elfsh_calloc(size_t, char);
void           *elfsh_realloc(void *, size_t);

void		elfsh_project_vectdim(elfshvector_t *vect, unsigned int *dim, 
				      unsigned int dimsz, unsigned long fct);
void*		elfsh_project_coords(elfshvector_t *vect, unsigned int *dim, 
				     unsigned int dimsz);
int		elfsh_recursive_vectalloc(unsigned long *tab, unsigned int *dims, 
					  unsigned int depth, unsigned int dimsz);
int		elfsh_register_vector(char *, void*, void*,
				      unsigned int *, unsigned int);

#endif
