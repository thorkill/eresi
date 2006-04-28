/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: libmjollnir-flow.h,v 1.2 2006-04-28 06:56:33 thor Exp $
 */


struct _mjrNaviMap {
 hash_t blocks;
};

struct _mjrBlock {
 u_int vaddr;
 u_int next;
 u_int alt;
 u_int ret;
};

typedef struct _mjrNaviMap MjrNaviMap;
typedef struct _mjrBlock MjrBlock;

