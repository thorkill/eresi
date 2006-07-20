
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: blocks.c,v 1.4 2006-07-20 17:27:11 thor Exp $
 *
 */

#include <libmjollnir.h>

mjr_block * mjr_create_block(u_int vaddr,char *section,u_int type) {
 mjr_block *nblk;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 XALLOC(nblk,sizeof(mjr_block),-1);

 nblk->vaddr = vaddr;
 nblk->type = type;
 nblk->name = NULL;
 nblk->section = strdup(section);
 nblk->flowTo = NULL;
 nblk->flowRet = NULL;
 nblk->listNext = NULL;
 nblk->listPrev = NULL;

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,(nblk));
}

