/*
 * 2001-2006 Devhell Labs
 * 2006 Asgard Labs, thorolf
 * 
 * Basic blocks related structures for libmjollnir
 *
 * $Id: libmjollnir-fprint.h,v 1.4 2007-01-09 17:14:23 thor Exp $
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
  mjrcaller_t		*parentfuncs;	/* Functions we are called from */
  mjrcaller_t		*childfuncs;	/* Functions we call */
  char			*md5;		/* MD5 Checksum */
}			mjrfunc_t;

#define ELFSH_SECTION_NAME_EDFMT_FUNCTIONS ".edfmt.functions"
