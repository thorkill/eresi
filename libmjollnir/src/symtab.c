
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: symtab.c,v 1.3 2006-07-10 20:21:51 thor Exp $
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

  if (n->type != MJR_TYPE_FUNCT && n->type != MJR_TYPE_SECT_START)
   continue;

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

int mjrSymbolDeleteByName(mjrSession *sess, char *symbol) {

 fprintf(D_DESC,"[__DEBUG__] mjrSymbolDeleteByName: <%s>\n", symbol);
 elfsh_remove_symbol(sess->obj->secthash[ELFSH_SECTION_SYMTAB], symbol);
 return 1;
}

int mjrSymbolDeleteByVaddr(mjrSession *sess, u_int *vaddr) {

 return 1;
}

/**
 * Rename symbol
 * FIXME:
 * you have to use -R at this moment couse blocks are not stored into binary itself
 * nor in external db.
 */
 
int mjrSymbolRename(mjrSession *sess,char *old_name,char *new_name) {

 mjrBlock *n;
 elfsh_Sym *sm;
 
 sm = elfsh_get_symbol_by_name(sess->obj, old_name);

 if (!sm)
  return 0;

#if __DEBUG__
 fprintf(D_DESC,"[__DEBUG__] mjrSymbolRename: %s (st_value: 0x%08x) -> %s\n", old_name, sm->st_value, new_name);
#endif
 
 n = hash_get(&sess->blocks, _vaddr2string(sm->st_value));

 if (n) {
  if (!mjrSymbolDeleteByName(sess,old_name))
   printf("Can't delete %s\n",old_name);
   mjrSymbolAdd(sess,n->section,n->vaddr,new_name);
 } else {
   fprintf(D_DESC,"[__DEBUG__] mjrSymbolRename: <%s> not found\n", old_name);
 }
 
 return 1;
}
