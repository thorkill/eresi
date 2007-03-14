/*
 * 2001-2006 Devhell Labs
 * 2006-2007 Asgard Labs, thorolf
 * 
 * Basic blocks related structures for libmjollnir
 *
 * $Id: libmjollnir-fprint.h,v 1.11 2007-03-14 18:37:57 strauss Exp $
 *
 */
#include "libmjollnir-blocks.h"

#define		MJR_FPRINT_TYPE_MD5	0

/* Abstract function representation */
typedef struct		s_function 
{
  elfsh_Addr	vaddr;		/* Function virtual address */
  u_int				size;			/* Function size */
  char				name[64];	/* Function name */
  mjrblock_t	*first;		/* First function block */
  char				md5[34];	/* MD5 Checksum */
}			mjrfunc_t;

