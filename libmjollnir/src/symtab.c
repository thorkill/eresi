/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: symtab.c,v 1.15 2007-03-14 18:37:57 strauss Exp $
 *
 */
#include <libmjollnir.h>



/* Rebuild the symtab with original names when possible */
int		mjr_symtab_rebuild(mjrsession_t *sess) 
{
 int		cn;
 int		x;
 char		**tab;
 mjrcontainer_t *tmp;
 mjrblock_t	*n;
 char		s[BSIZE];

 tab = hash_get_keys(&sess->cur->blkhash, &cn);
 for (x = 0; x < cn; x++) 
   {
     tmp = hash_get(&sess->cur->blkhash, tab[x]);
		 n = tmp->data;
     fprintf(D_DESC,"[__DEBUG_MJOLLNIR__] mjr_symtab_rebuild: "XFMT" \n",
	     n->vaddr);
     if (!mjr_block_funcstart(tmp))
       continue;
     snprintf(s, BSIZE, "%s"AFMT, 
	      (char *) config_get_data(MJR_COFING_CALL_PREFIX), n->vaddr);
     mjr_symbol_add(sess, n->vaddr, s);
   }
 return 1;
}

/* This function inserts new symbol as a function. Shortcut for insert/set */
int		mjr_symbol_add(mjrsession_t	*sess, 
							       elfsh_Addr		vaddr, 
			  				     char					*fname)
{
  elfshsect_t	*sect;
  elfsh_Sym	sym;
  
  sect = elfsh_get_parent_section(sess->cur->obj, vaddr, NULL);

#if __DEBUG_MJOLLNIR__
  fprintf(D_DESC,"[__DEBUG_MJOLLNIR__] mjrSymbolAdd: [%10s] 0x%08x <%s>\n",
	  "SECNAME", vaddr, fname);
#endif

  if (!sect)
    return 0;
  sym = elfsh_create_symbol(vaddr, 0, STT_FUNC, 0, 0, sect->index);
  elfsh_insert_symbol(sess->cur->obj->secthash[ELFSH_SECTION_SYMTAB], &sym, fname);
  return 1;
}


/* Remove symbol by name */
int		mjr_symbol_delete_by_name(mjrsession_t *sess, char *symbol) 
{ 
#if __DEBUG_MJOLLNIR__
  fprintf(D_DESC,"[__DEBUG_MJOLLNIR__] mjr_symbol_deleteByName: <%s>\n", symbol);
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
  
  sm = elfsh_get_symbol_by_name(sess->cur->obj, old_name);
  if (!sm)
    return 0;
  
#if __DEBUG_MJOLLNIR__
 fprintf(D_DESC,
	 "[__DEBUG_MJOLLNIR__] mjr_symbol_rename: %s (st_value: 0x%08x) -> %s \n", 
	 old_name, sm->st_value, new_name);
#endif
 
 mjr_symbol_add(sess, sm->st_value, new_name);
 mjr_symbol_delete_by_name(sess, old_name); 
 return 1;
}
