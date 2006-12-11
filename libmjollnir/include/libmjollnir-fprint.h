/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: libmjollnir-fprint.h,v 1.2 2006-12-11 13:57:34 may Exp $
 *
 */

#define MJR_FNG_TYPE_MD5 0

typedef struct _mjrFunction mjrfunction_t;

/* Here comes linked list of conditional blocks
   found between this vaddr of this function
   and next vaddr of a function */
struct	_mjrFunction 
{
  u_int	vaddr;
  char	*md5;
};

