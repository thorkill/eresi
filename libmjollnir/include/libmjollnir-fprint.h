/*
 * 2001-2006 Devhell Labs
 * 2006 Asgard Labs, thorolf
 * 
 * Basic blocks related structures for libmjollnir
 *
 * $Id: libmjollnir-fprint.h,v 1.3 2007-01-02 06:21:23 may Exp $
 *
 */
#include "libmjollnir-blocks.h"

#define		MJR_FPRINT_TYPE_MD5	0

/* Abstract function representation */
typedef struct		s_function 
{
  elfsh_Addr		vaddr;		/* Function virtual address */
  u_int			size;		/* Function size */
  char			*name;		/* Function name */
  mjrblock_t		*first;		/* First function block */
  hash_t		parentfuncs;	/* Functions we are called from */
  hash_t		childfuncs;	/* Functions we call */
  char			*md5;		/* MD5 Checksum */
}			mjrfunc_t;
