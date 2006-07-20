/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: libmjollnir-fprint.h,v 1.1 2006-07-20 17:28:47 thor Exp $
 *
 */

#define MJR_FNG_TYPE_MD5 0

typedef struct _mjrFunction mjrFunction;

struct _mjrFunction {
 u_int vaddr;
 char *md5;
 /* here comes linked list of conditional blocks
  found between this vaddr of this function
  and next vaddr of a function bla bla */
};

