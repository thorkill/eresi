
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: blocks.c,v 1.1 2006-07-08 21:24:19 thor Exp $
 *
 */

#include <libmjollnir.h>

mjrBlock * mjrCreateBlock(u_int vaddr,char *section,u_int type) {
 mjrBlock *nblk;

 ELFSH_NOPROFILE_IN();
 
 XALLOC(nblk,sizeof(mjrBlock),-1);

 nblk->vaddr = vaddr;
 nblk->type = type;
 nblk->section = strdup(section);
 nblk->flowTo = NULL;
 nblk->flowRet = NULL;
 nblk->listNext = NULL;
 nblk->listPrev = NULL;

 ELFSH_NOPROFILE_ROUT(nblk);
}

