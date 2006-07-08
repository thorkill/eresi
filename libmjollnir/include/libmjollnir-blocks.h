
/* this covers at this moment only calls */

#define MJR_TYPE_SECT_START 0
#define MJR_TYPE_FUNCT		1

typedef struct _mjrBlock mjrBlock;

struct _mjrBlock {
 u_int vaddr;
 u_int type;
 u_int flowTo;
 u_int flowRet;
 char *section;
 struct mjrBlock *listNext;
 struct mjrBlock *listPrev;
};
