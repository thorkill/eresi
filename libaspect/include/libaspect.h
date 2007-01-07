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

#ifdef  __BEOS__
 #include <image.h>
 #include <bsd_mem.h>
#else
 #define __USE_GNU
 #include <dlfcn.h>
#endif

/* Endianess definitions */
#if defined(__linux__) || defined (__BEOS__)
 #include <endian.h>
#elif defined(__FreeBSD__) || defined (__OpenBSD__) || defined(__NetBSD__)
#include <machine/endian.h>
 #define __LITTLE_ENDIAN _LITTLE_ENDIAN
 #define __BIG_ENDIAN    _BIG_ENDIAN
 #if __FreeBSD__ < 5
  #define __BYTE_ORDER	  __LITTLE_ENDIAN
 #else
  #define __BYTE_ORDER    BYTE_ORDER
 #endif
#elif defined(sun)
 #define __LITTLE_ENDIAN 1234
 #define __BIG_ENDIAN	 4321
#if !defined(__i386)
 #define __BYTE_ORDER    __BIG_ENDIAN
#else
 #define __BYTE_ORDER    __LITTLE_ENDIAN
#endif
#elif defined(HPUX)
 #include <arpa/nameser_compat.h>
 #undef ADD
 #define __LITTLE_ENDIAN LITTLE_ENDIAN
 #define __BIG_ENDIAN    BIG_ENDIAN
 #define __BYTE_ORDER    BYTE_ORDER
#elif defined(IRIX)
 #include <standards.h>
 #include <sys/endian.h>
 #define __LITTLE_ENDIAN LITTLE_ENDIAN
 #define __BIG_ENDIAN    BIG_ENDIAN
 #define __BYTE_ORDER    BIG_ENDIAN
#endif


#ifndef swap32
#define swap32(x)						\
	((uint32_t)(						\
	(((uint32_t)(x) & (uint32_t) 0x000000ffU) << 24) |	\
	(((uint32_t)(x) & (uint32_t) 0x0000ff00U) <<  8) |	\
	(((uint32_t)(x) & (uint32_t) 0x00ff0000U) >>  8) |	\
	(((uint32_t)(x) & (uint32_t) 0xff000000U) >> 24) ))
#endif

#ifndef swap16
#define swap16(x)							\
	((unsigned short)(						\
	(((unsigned short)(x) & (unsigned short) 0x00ffU) << 8) |	\
	(((unsigned short)(x) & (unsigned short) 0xff00U) >> 8) ))
#endif

#ifndef swap64
#define	swap64(x)						    \
	((uint64_t)(						    \
	((uint64_t)((uint64_t)(x) & (uint64_t) 0x00000000000000ffU) << 56) | \
	((uint64_t)((uint64_t)(x) & (uint64_t) 0x000000000000ff00U) << 40) | \
	((uint64_t)((uint64_t)(x) & (uint64_t) 0x0000000000ff0000U) << 24) | \
	((uint64_t)((uint64_t)(x) & (uint64_t) 0x00000000ff000000U) << 8)  | \
	((uint64_t)((uint64_t)(x) & (uint64_t) 0x000000ff00000000U) >> 8)  | \
	((uint64_t)((uint64_t)(x) & (uint64_t) 0x0000ff0000000000U) >> 24) | \
	((uint64_t)((uint64_t)(x) & (uint64_t) 0x00ff000000000000U) >> 40) | \
	((uint64_t)((uint64_t)(x) & (uint64_t) 0xff00000000000000U) >> 56)))
#endif


/* When we link inside the elfsh project, dont forget to redirect allocations */
#if defined(ASPECT_ELFSH_INTERN)
void           *elfsh_malloc(size_t);
void           *elfsh_calloc(size_t, char);
void           *elfsh_realloc(void *, size_t);
void		elfsh_free(void *tofree);

 #define	malloc  elfsh_malloc
 #define	calloc  elfsh_calloc
 #define	realloc elfsh_realloc
 #define	free	elfsh_free
#endif

/* Include this here since it contains an allocation too */
#include "libhash.h"
#include "libbtree.h"

/* The structure of multidimensional vectors */
typedef struct	s_vector
{
  void		*hook;			/* The vector data */
  void		*register_func;		/* Registration function */
  void		*default_func;		/* Default registered function */
  u_int		*arraydims;		/* Size of each dimension */
  char		**strdims;		/* Label for each dimension */
  u_int		arraysz;		/* Number of dimensions */
#define		VECTOR_TYPE_UNKNOWN	0
#define		VECTOR_TYPE_HASH	1
#define		VECTOR_TYPE_FUNC	2
  u_int		type;			/* Type of elements in this vector */
}		vector_t;

/* Initialize the vectors of libaspect */
void		aspect_vectors_init();

/* Retreive pointer on the vector hash table */
hash_t*		aspect_vecthash_get();

/* Retreive a vector from the vectors hash table */
vector_t*	aspect_vector_get(char *name);

/* Add the function pointer to the hook at requested coordonates */
void		aspect_vectors_insert(vector_t *vect, unsigned int *dim, 
				      unsigned long fct);

/* Get the hook from the dimension array */
void*		aspect_vectors_select(vector_t *vect, unsigned int *dim);

/* Add a new vector */
int		aspect_register_vector(char *, void*, unsigned int*, char **,
				       unsigned int, unsigned int);

#endif
