
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: symtab.c,v 1.1 2006-07-08 21:24:21 thor Exp $
 *
 */

#include <libmjollnir.h>

int mjrSymtabRebuild(mjrSession *sess) {

 int cn,x;
 char **tab;
 mjrBlock *n;
 char s[BSIZE];

 tab = hash_get_keys(&sess->blocks,&cn);

 for(x=0;x<cn;x++) {
  n = hash_get(&sess->blocks,tab[x]);
  memset(s,0x00,BSIZE);
  snprintf(s,BSIZE,"%s%s", MJR_CALL_PREFIX,(char *)_vaddr2string(n->vaddr));
  mjrSymbolAdd(sess,n->section,n->vaddr,s);
 }

 return 1;
}

/**
 * This function inserts new symbol as a function.
 * shortcut for insert/set
 */

int mjrSymbolAdd(mjrSession *sess, char *section, u_int vaddr, char *fname)
{
	elfshsect_t *sct;
	elfsh_Sym	sym;

#if __DEBUG__
  fprintf(D_DESC,"[__DEBUG__] mjrSymbolAdd: [%10s] 0x%08x <%s>\n",
   section, vaddr, fname);
#endif

	sct = elfsh_get_section_by_name((elfshobj_t *)sess->obj, (char *)section, NULL, NULL, NULL);
	
	if (!sct) {
	 return 0;
	}

	sym = elfsh_create_symbol(vaddr, 0, STT_FUNC, 0, 0, sct->index);
	elfsh_insert_symbol(sess->obj->secthash[ELFSH_SECTION_SYMTAB], &sym, fname);

	return 1;
}
