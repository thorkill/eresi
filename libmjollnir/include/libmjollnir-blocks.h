/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: libmjollnir-blocks.h,v 1.4 2006-07-20 17:27:10 thor Exp $
 *
 */

/* this covers at this moment only calls */

#define MJR_TYPE_SECT_START 0
#define MJR_TYPE_FUNCT		1

typedef struct _mjrBlock mjr_block;

struct _mjrBlock {
 u_int vaddr;
 u_int type;
 char *name;
 u_int flowTo;
 u_int flowRet;
 char *section;
 struct mjrBlock *listNext;
 struct mjrBlock *listPrev;
};
