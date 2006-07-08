
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: libmjollnir.h,v 1.4 2006-07-08 21:24:16 thor Exp $
 *
 */

#include <libelfsh.h>
#include <libasm.h>

/* size of temp buffers */
#define BSIZE 4096 

/* default subroutines prefix */
#define MJR_CALL_PREFIX		"sub_"

typedef struct _mjrSession mjrSession;

struct _mjrSession {

 elfshobj_t *obj;     	/* elfsh object */
 asm_processor proc;  	/* proc */
 u_int curVaddr;      	/* current vaddr (while searching/analizing...) */
 hash_t blocks;	  		/* blocks */
 asm_instr  ihist[4]; 	/* instruction history */

};

#include "libmjollnir-blocks.h"
#include "libmjollnir-int.h"

mjrBlock * mjrCreateBlock(u_int vaddr,char *section,u_int type);

int mjrAnalize(mjrSession *sess,int flags);
int mjrFindCalls(mjrSession *sess,char *section_name);

int mjrGetCallDst(mjrSession *sess,int *dest);

int mjrHistoryUpdate(mjrSession *x, asm_instr inst);

int mjrInitSession(mjrSession *sess);
int mjrSetupProcessor(mjrSession *sess);

char *_vaddr2string(u_int vaddr);

int mjrSymtabRebuild(mjrSession *sess);
int mjrSymbolAdd(mjrSession *sess, char *section, u_int vaddr, char *fname);
