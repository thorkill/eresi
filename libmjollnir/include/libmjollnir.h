
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: libmjollnir.h,v 1.5 2006-07-15 17:06:06 thor Exp $
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

/* blocks.c */
mjr_block * mjr_create_block(u_int,char *,u_int);

/* core.c */
int mjr_analize(mjrSession *,int);
int mjr_find_calls(mjrSession *,char *);

/* dstCall.c */
int mjr_get_call_dst(mjrSession *,int *);

/* history.c */
int mjr_history_update(mjrSession *, asm_instr);

/* init.c */
int mjr_init_session(mjrSession *);
int mjr_setup_processor(mjrSession *);

/* internal.c */
char *_vaddr2string(u_int);

/* symtab.c */
int mjr_symtab_rebuild(mjrSession *);
int mjr_symbol_add(mjrSession *, char *, u_int, char *);
int mjr_symbol_delete_by_name(mjrSession *, char *);
int mjr_symbol_rename(mjrSession *,char *,char *);

