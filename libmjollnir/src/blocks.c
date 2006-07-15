
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: blocks.c,v 1.3 2006-07-15 17:06:07 thor Exp $
 *
 */

#include <libmjollnir.h>

mjr_block * mjr_create_block(u_int vaddr,char *section,u_int type) {
 mjr_block *nblk;

 ELFSH_NOPROFILE_IN();

 XALLOC(nblk,sizeof(mjr_block),-1);

 nblk->vaddr = vaddr;
 nblk->type = type;
 nblk->name = NULL;
 nblk->section = strdup(section);
 nblk->flowTo = NULL;
 nblk->flowRet = NULL;
 nblk->listNext = NULL;
 nblk->listPrev = NULL;

 ELFSH_NOPROFILE_ROUT(nblk);
}

