
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: libmjollnir-int.h,v 1.1.1.1 2006-02-20 23:25:27 thor Exp $
 */

#define __DEBUG__ 0

void * mjr_fingerprint_function(unsigned char *buff,int type, int mlen);

/* this is very beta early version - so don't panic  WIP*/
int mjr_find_calls_raw(elfshobj_t *,hash_t *,char *,char *,int *);
int mjr_find_calls_symtab(elfshobj_t *,hash_t *,int *);

/* libmjollnir.c */
int mjr_i386_cut_function(unsigned char *src,unsigned char *dst, int mlen);
int mjr_add_symbol(elfshobj_t *obj,char *section,u_int vaddr,char *fname);

/* fngdb.c */
int mjr_load_md5_db(char *,hash_t *);

