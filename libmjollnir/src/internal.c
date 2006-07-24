
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: internal.c,v 1.4 2006-07-24 18:50:02 thor Exp $
 *
 */

#include <libmjollnir.h>

char *_vaddr2string(u_int vaddr) {
 char *str;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 XALLOC(str,BSIZE , NULL);
 memset(str, 0x00, BSIZE);
 snprintf(str, BSIZE - 1, "0x%08x", vaddr);

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,(char *)(str));

}
