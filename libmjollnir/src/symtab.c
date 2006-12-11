/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: symtab.c,v 1.6 2006-12-11 13:57:39 may Exp $
 *
 */
#include <libmjollnir.h>



/* Rebuild the symtab with original names when possible */
int		mjr_symtab_rebuild(mjrsession_t *sess) 
{
 int		cn, x;
 char		**tab;
 mjrblock_t	*n;
 char		s[BSIZE];

 tab = hash_get_keys(&sess->cur->blocks,&cn);
 for (x=0;x<cn;x++) 
   {
     n = hash_get(&sess->cur->blocks,tab[x]);
     if (n->type != MJR_TYPE_FUNCT && n->type != MJR_TYPE_SECT_START)
       continue;
     memset(s,0x00,BSIZE);
     snprintf(s,BSIZE,"%s%s", MJR_CALL_PREFIX,(char *)_vaddr2string(n->vaddr));
     mjr_symbol_add(sess,n->section,n->vaddr,s);
   }
 return 1;
}

/* This function inserts new symbol as a function. Shortcut for insert/set */
int		mjr_symbol_add(mjrsession_t	*sess, 
			       char		*section, 
			       u_int		vaddr, 
			       char		*fname)
{
  elfshsect_t	*sct;
  elfsh_Sym	sym;

#if __DEBUG__
  fprintf(D_DESC,"[__DEBUG__] mjrSymbolAdd: [%10s] 0x%08x <%s>\n",
	  section, vaddr, fname);
#endif
  
  sct = elfsh_get_section_by_name((elfshobj_t *) sess->cur->obj, 
				  (char *) section, NULL, NULL, NULL);
  if (!sct)
    return 0;
  sym = elfsh_create_symbol(vaddr, 0, STT_FUNC, 0, 0, sct->index);
  elfsh_insert_symbol(sess->cur->obj->secthash[ELFSH_SECTION_SYMTAB], &sym, fname);
  return 1;
}


/* Remove symbol by name */
int		mjr_symbol_delete_by_name(mjrsession_t *sess, char *symbol) 
{ 
#if __DEBUG__
  fprintf(D_DESC,"[__DEBUG__] mjr_symbol_deleteByName: <%s>\n", symbol);
#endif
  
  elfsh_remove_symbol(sess->cur->obj->secthash[ELFSH_SECTION_SYMTAB], symbol);
  return 1;
}

/* Rename symbol FIXME */
int		mjr_symbol_rename(mjrsession_t  *sess, 
				  char		*old_name, 
				  char		*new_name) 
{
  elfsh_Sym	*sm;
  elfshsect_t	*s;
  
  sm = elfsh_get_symbol_by_name(sess->cur->obj, old_name);
  if (!sm)
    return 0;
  s = elfsh_get_section_by_index(sess->cur->obj, sm->st_shndx, NULL, NULL);
  
#if __DEBUG__
 fprintf(D_DESC,
	 "[__DEBUG__] mjr_symbol_rename: %s (st_value: 0x%08x) -> %s <%s>\n", 
	 old_name, sm->st_value, new_name, s->name);
#endif
 
 mjr_symbol_add(sess,s->name,sm->st_value,new_name);
 mjr_symbol_delete_by_name(sess,old_name); 
 return 1;
}

