
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: internal.c,v 1.1 2006-07-08 21:24:21 thor Exp $
 *
 */

#include <libmjollnir.h>

char *_vaddr2string(u_int vaddr) {
 char *str;

 ELFSH_NOPROFILE_IN();

 XALLOC(str,BSIZE ,-1);
 memset(str, 0x00, BSIZE);
 snprintf(str, BSIZE - 1, "0x%x", vaddr);

 ELFSH_NOPROFILE_ROUT(str);

}
