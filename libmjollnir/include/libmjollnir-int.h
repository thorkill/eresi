
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: libmjollnir-int.h,v 1.1.1.2 2006-02-23 22:42:25 thor Exp $
 */

#define __DEBUG__ 0

void * mjr_fingerprint_function(unsigned char *buff,int type, int mlen);

/* this is very beta early version - so don't panic  WIP*/
int mjr_find_calls_raw(Mjr_CTX *,hash_t *,char *,char *);
int mjr_find_calls_symtab(elfshobj_t *,hash_t *);

/* libmjollnir.c */
int mjr_i386_cut_function(unsigned char *src,unsigned char *dst, int mlen);
int mjr_add_symbol(elfshobj_t *obj,char *section,u_int vaddr,char *fname);

/* fngdb.c */
int mjr_load_md5_db(char *,hash_t *);

