
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: libmjollnir-int.h,v 1.1.1.3 2006-03-06 22:35:22 thor Exp $
 */

#define __DEBUG__ 				0
#define __DEBUG_READ__			0
#define __DEBUG_OPERAND__		0

/* debug output stdout/stderr */
#define D_DESC stdout

void * mjr_fingerprint_function(Mjr_CTX *,unsigned char *buff,int type, int mlen);

/* this is very beta early version - so don't panic  WIP*/
int mjr_find_calls_raw(Mjr_CTX *,hash_t *,char *,char *);
int mjr_find_calls_symtab(Mjr_CTX *,hash_t *);

/* libmjollnir.c */
int mjr_i386_cut_function(Mjr_CTX *,unsigned char *src,unsigned char *dst, int mlen);
int mjr_add_symbol(elfshobj_t *,char *section,u_int vaddr,char *fname);
int mjr_ihist(Mjr_CTX *,asm_instr);
int mjr_call_dest(Mjr_CTX *, int *);
char *asm_display_instr_att(asm_instr *,int);

/* fngdb.c */
int mjr_load_md5_db(char *,hash_t *);


/* external stuff */
void  *elfsh_malloc(size_t);
void  elfsh_free(void *);

