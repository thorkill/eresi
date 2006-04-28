/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de 
 * BSD License 
 * $Id: bifrost.c,v 1.2 2006-04-28 06:56:33 thor Exp $
 *
 */

/*
 Development overview:
  I'm gona to do something like this:
   _______________________
  /Binary/naviMap
  | |-> functions
  | \____blocks of instructions


 All blocks are stored into hash table with
 a reference to next, altpath and ret vaddr.

 A function will contain a linked list of
 all called by this function blocks and functions
 for better cross referencing.

 We should be able to dump a function, few blocks
 in a sequence or whole object like modflow does.

 Besides we should be able to compute x/y lenght of
 a function (what is widht/depth is not clear at
 this time - need research) and lookup n'th function
 or block from current or requested one - this should
 be used for fingerprinting.

*/

#include <libmjollnir.h>

int mjrNaviMapInit(MjrNaviMap *m) {
 hash_init(&m->blocks,MJR_MAX_BLOCKS);
 return 1;
}

MjrBlock *mjrNaviCreateBlock(u_int vaddr) {
 MjrBlock *b;
 b = elfsh_malloc(sizeof(MjrBlock));
 b->vaddr = vaddr;
 b->next = NULL;
 b->alt = NULL;
 b->ret = NULL;

 return b;
}

int mjrNaviAddBlock(MjrNaviMap *map,MjrBlock *bl) {
 char *tmp;
 tmp = elfsh_malloc(BSIZE);
 memset(tmp,0,BSIZE);
 snprintf(tmp,BSIZE-1,"0x%x",bl->vaddr);
 printf("Add %s\n",tmp);
 if (hash_add(&map->blocks,tmp,&bl) != 0) {return 0;}

 return 1;
}

void mjrNaviDump(MjrNaviMap *map,u_int vaddr) {
 printf("Dump 0x%x\n",vaddr);
 hash_print(&map->blocks);
 return;
}


