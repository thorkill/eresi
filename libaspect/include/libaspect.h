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
#if !defined(__USE_GNU)
 #define __USE_GNU
#endif
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

/* Include this here since it contains an allocation too */
#include "libaspect-hash.h"
#include "libaspect-btree.h"
#include "libaspect-profiler.h"

/* Those types are only the builtin types */
#define         ASPECT_TYPE_UNKNOW       0  /* Unknown           */
#define         ASPECT_TYPE_RAW          1  /* Raw               */
#define		ASPECT_TYPE_BYTE	 2  /* Byte	         */
#define         ASPECT_TYPE_STR          3  /* String            */
#define         ASPECT_TYPE_SHORT        4  /* 2 bytes           */
#define         ASPECT_TYPE_INT          5  /* 4 bytes		 */
#define         ASPECT_TYPE_LONG         6  /* 4 or 8 bytes      */
#define         ASPECT_TYPE_DADDR        7  /* 4 or 8 bytes      */
#define         ASPECT_TYPE_CADDR        8  /* 4 or 8 bytes      */
#define	        ASPECT_TYPE_VECT	 9  /* Vector type	 */
#define		ASPECT_TYPE_HASH	10  /* Hash table type	 */
#define         ASPECT_TYPE_BASENUM     11  /* BASE TYPES NUMBER */

#define		ASPECT_TYPENAME_UNKNOW	"unknown"
#define		ASPECT_TYPENAME_RAW	"raw"     
#define		ASPECT_TYPENAME_BYTE	"byte"    
#define		ASPECT_TYPENAME_STR	"string"  
#define		ASPECT_TYPENAME_SHORT	"short"   
#define		ASPECT_TYPENAME_INT	"int"     
#define		ASPECT_TYPENAME_LONG	"long"    
#define		ASPECT_TYPENAME_DADDR	"daddr"   
#define		ASPECT_TYPENAME_CADDR	"caddr"   
#define		ASPECT_TYPENAME_VECT	"vector" 
#define		ASPECT_TYPENAME_HASH	"hash"

/* A structure for the type information */
typedef struct		s_info
{
  char			*name;
  u_int			size;
}			typeinfo_t;


/* Structure for type */
typedef struct		s_type
{
  u_int			type;		/* Plain or pointed type */

  u_char		isptr;		/* the type is a pointer */
  u_int			size;		/* Type full memsize     */
  u_int			off;		/* Offset inside parent  */

  u_int			dimnbr;		/* Nbr of array dimensions */
  u_int			*elemnbr;	/* Nbr of elements per dim */
  char			*name;		/* Type name             */
  char			*fieldname;	/* Field name		 */

  /* We have a list of child, and a list
  ** of nexts in case we are ourselves 
  ** a child of a structure object */
  struct s_type		*childs;	/* Curobj fields if any */
  struct s_type		*next;		/* Next parent field */
}			aspectype_t;


/* The structure of multidimensional vectors */
typedef struct	s_vector
{
  void		*hook;		   /* The vector data */
  void		*register_func;	   /* Registration function */
  void		*default_func;	   /* Default registered function */
  u_int		*arraydims;	   /* Size of each dimension */
  char		**strdims;	   /* Label for each dimension */
  u_int		arraysz;	   /* Number of dimensions */
  u_int   	type;		   /* Elements type in this vector */
}		vector_t;


/* Config related data types */
#define		CONFIG_HASH_SIZE	256

/* Default names for config names */
#define		CONFIG_NAME_SAFEMODE	"safemode"

/* Config flags */
#define		CONFIG_SAFEMODE_OFF	0
#define		CONFIG_SAFEMODE_ON	1


/* A configuration item */
typedef struct	s_config_item
{
  char		*name;
#define		CONFIG_TYPE_INT	0
#define		CONFIG_TYPE_STR	1
  u_int		type;			  /* int will use val, str *data */
  
  /* RO/RW - it's relevant to higher api
   like vm_ allows direct updates to those values
   when RW is set and enforces usage of vm_api
   when RO is set (see profile) */
  u_int		val;			  /* For int values 0-off/1-on ... */
#define		CONFIG_MODE_RW	0
#define		CONFIG_MODE_RO	1
  u_int		mode;			  
  void		*data;
}		configitem_t;



/* The library has its world itself */
/* There stands all the flags that are used by libelfsh */
typedef struct	s_aspectworld
{
  hash_t	config_hash;		/* Configuration */

#define		PROFILE_NONE	0
#define		PROFILE_WARN    (1 << 0)
#define		PROFILE_FUNCS   (1 << 1)
#define		PROFILE_ALLOC   (1 << 2)
#define		PROFILE_DEBUG	(1 << 3)
  u_char	proflevel;		/* Profiling switch */
  u_char	profstarted;		/* Profiling started ? */
  int           (*profile)(char *);	/* Profiling output func */
  int           (*profile_err)(char *);	/* Profiling error func */
  
  /* Libui pointers */
  void	     	(*endline)();

  /* Simple */
  char		*(*colorinstr)(char *text);
  char        	*(*colorstr)(char *t);
  char         	*(*colorfieldstr)(char *t);
  char         	*(*colortypestr)(char *t);
  char		*(*colorend)(char *text); 
  char		*(*colorwarn)(char *text);
  char		*(*colorfunction)(char *text);
  char		*(*colorfilename)(char *text);
  /* Advanced */
  char 		*(*coloradv)(char *ty, char *p, char *te);
  char		*(*colorinstr_fmt)(char* p, char *t);
  char          *(*coloraddress)(char *p, u_long a);
  char          *(*colornumber)(char *p, u_int n);
  char          *(*colorstr_fmt)(char *p, char *t);
  char          *(*colorfieldstr_fmt)(char *p, char *t);
  char          *(*colortypestr_fmt)(char *p, char *t);
  char		*(*colorwarn_fmt)(char *pattern, char *text);
}		aspectworld_t;

/* Make it accessible from all files */
extern aspectworld_t	aspectworld;


/* Initialize the library */
int		aspect_init();

/* Retreive pointer on the vector hash table */
hash_t*		aspect_vecthash_get();

/* Retreive a vector from the vectors hash table */
vector_t*	aspect_vector_get(char *name);

/* Add the function pointer to the hook at requested coordonates */
void		aspect_vectors_insert(vector_t *vect, 
				      unsigned int *dim, 
				      unsigned long fct);

/* Get the hook from the dimension array */
void*		aspect_vectors_select(vector_t *vect, 
				      unsigned int *dim);

/* Project each dimension and get the requested data pointer */
void            *aspect_vectors_selectptr(vector_t *vect, 
					  unsigned int *dim);

/* Add a new vector */
int		aspect_register_vector(char *, void*, 
				       unsigned int*,
				       char **, unsigned int, 
				       unsigned int);

/* Type related functions */
char		*aspect_typename_get(u_int type);
int		aspect_basetypes_create();
int		aspect_type_register(char *label, 
				     char **fields, 
				     u_int fieldnbr);
aspectype_t	*aspect_type_copy(aspectype_t	*type, 
				  unsigned int	off, 
				  u_char	isptr, 
				  u_int		elemnbr, 
				  char		*fieldname,
				  u_int		*dims);
int		aspect_basetype_register(char *name, u_int size);

/* profile.c : Profiler related functions */
void		profiler_reset(u_int sel);
int		profiler_print(char *file, char *func, 
			       u_int line, char *msg);
void		profiler_err(char *file, char *func, 
			     u_int line, char *msg);
void		profiler_out(char *file, char *func, u_int line);
void		profiler_error();
void		profiler_incdepth();
void		profiler_decdepth();
void		profiler_updir();
int		profiler_enabled();
int		profiler_is_enabled(u_char mask);
u_char		profiler_started();

int		profiler_enable_err();
int		profiler_enable_out();
int		profiler_enable_alloc();
int		profiler_enable_debug();
int		profiler_disable_err();
int		profiler_disable_out();
int		profiler_disable_alloc();
int		profiler_disable_debug();
int		profiler_disable_all();
int		profiler_enable_all();

void		profiler_install(int (*profile)(char *), 
				 int (*profile_err)(char *));

/* The allocation profiler */
void			profiler_alloc_warnprint(char *s, int fat, int i);
void			profiler_alloc_warning(u_char warntype);
void			profile_alloc_shift();
int			profiler_alloc_update(char *file, char *func, 
					      u_int line, u_long addr, 
					      u_char atype, u_char otype);
profallocentry_t	*profiler_alloc_find(u_char direction,
					     u_long addr, 
					     u_char optype);

/* Color related handlers for the profiler */
void		profiler_setcolor(void (*endline)(), 
				  char *(*colorinstr)(char *text),
				  char *(*colorstr)(char *t), 
				  char *(*colorfieldstr)(char *t),
				  char *(*colortypestr)(char *t), 
				  char *(*colorend)(char *text),
				  char *(*colorwarn)(char *text), 
				  char *(*colorfunction)(char *text),
				  char *(*colorfilename)(char *text));
void		profiler_setmorecolor(char *(*coloradv)(char *ty, 
							char *p, 
							char *te),
				      char *(*colorinstr_fmt)(char* p, 
							      char *t),
				      char *(*coloraddress)(char *p,
							    u_long a),
				      char *(*colornumber)(char *p, 
							   u_int n),
				      char *(*colorstr_fmt)(char *p, 
							    char *t),
				      char *(*colorfieldstr_fmt)(char *p,
								 char *t),
				      char *(*colortypestr_fmt)(char *p,
								char *t),
				      char *(*colorwarn_fmt)(char *pat,
							     char *text));

/* config.c : Configuration related fonctions */
void		config_init();
void		config_add_item(char *name, u_int type, 
				u_int mode, void *dat);
void		config_update_key(char *name, void *data);
void		*config_get_data(char *name);
void 		config_safemode_set();
void		config_safemode_reset();
int		config_safemode();


#endif
