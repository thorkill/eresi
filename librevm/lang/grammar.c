/**
 * @file grammar.c
 * @ingroup librevm_lang
 * @brief We dont use bison and have our own parser generator.
 *
 * Started on  Sun Feb  9 22:57:58 2003 jfv
 * $Id: grammar.c,v 1.21 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "revm.h"


/**
 * @brief Get a va_list of parameters 
 */
static int	parse_lookup_varlist(char *param, char *fmt, ...)
{
  int		rc;
  va_list	arg_ptr;

  va_start(arg_ptr, fmt);
  rc = vsscanf(param, fmt, arg_ptr);
  va_end(arg_ptr);
  return (rc);
}


/** 
 * @brief Parse a vector access 
 */
revmobj_t	*parse_vector(char *param, char *fmt)
{
  unsigned int		size;
  char		index[ERESI_MEANING];
  vector_t	*cur;
  int		dimnbr;
  unsigned int	*dims;
  revmobj_t	*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  size = parse_lookup_varlist(param, fmt, index);
  if (size != 1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Parser handling failed", NULL);
  if (!strchr(index, ':'))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Index parser failed", NULL);

  /* Grab the vector */
  dimnbr = revm_vectors_getdimnbr(index);
  dims = alloca(dimnbr * sizeof(unsigned int));
  revm_vectors_getdims(index, dims);
  cur = aspect_vector_get(index);

  /* Early sanity checks */
  if (!cur)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown requested vector", NULL);
  if (revm_vector_bad_dims(cur, dims, dimnbr))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Requested vector with bad dimensions", NULL);

  /* Get a pointer on the desired entry of the vector */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,ret, sizeof(revmobj_t), NULL);
  ret->parent   = aspect_vectors_selectptr(cur, dims);
  ret->otype    = aspect_type_get_by_id(cur->type);
  ret->perm     = 1;
  ret->immed    = 0;
  ret->get_obj  = (void *) revm_generic_getobj;
  ret->set_obj  = (void *) revm_long_setobj;

  /* Success */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/** 
 * @brief Parse a hash access 
 */
revmobj_t	*parse_hash(char *param, char *fmt)
{
  unsigned int		size;
  char		index[ERESI_MEANING];
  hash_t	*hash;
  revmobj_t	*ret;
  char		*entryname;
  char		*hashname;
  void		*ptr;
  revmexpr_t	*expr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  size = parse_lookup_varlist(param, fmt, index);
  if (size != 1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Parser handling failed", NULL);
  
  /* Get hash table and entry */
  entryname = strchr(index, ':');
  if (entryname)
    *entryname++ = 0x00;

  /* Resolve hash name without messing with variable type */
  expr = revm_expr_get(index);
  if (expr)
    {
      if (!expr->type || !expr->value)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Parser handling failed", NULL);
      ret = revm_copy_object(expr->value);	  
      if (revm_convert_object(expr, ASPECT_TYPE_STR) < 0)
	{
	  revm_destroy_object(expr->value);
	  expr->value = ret;
	  expr->type  = ret->otype;
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unknown key for source object", NULL);
	}
      hashname = strdup(expr->value->immed_val.str);
      revm_destroy_object(expr->value);
      expr->value = ret;
      expr->type  = ret->otype;
    }
  else
    hashname = revm_lookup_string(index);
  
  /* Resolve entry name without messing with variable type */
  if (entryname)
    {
      expr = revm_expr_get(entryname);
      if (expr)
	{
	  if (!expr->type || !expr->value)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Parser handling failed", NULL);
	  ret = revm_copy_object(expr->value);	  
	  if (revm_convert_object(expr, ASPECT_TYPE_STR) < 0)
	    {
	      revm_destroy_object(expr->value);
	      expr->value = ret;
	      expr->type = ret->otype;
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			   "Unknown key for source object", NULL);
	    }
	  entryname = strdup(expr->value->immed_val.str);
	  revm_destroy_object(expr->value);
	  expr->value = ret;
	  expr->type  = ret->otype;
	}
      else
	entryname = revm_lookup_string(entryname);
    }

  /* In case the hash table does not exist, create it empty */
  hash = hash_find(hashname);
  if (!hash)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     hash, sizeof(hash_t), NULL);
      hash_init(hash, strdup(hashname), 7, ASPECT_TYPE_UNKNOW);
    }

  /* Now deal with the entry */
  ptr = (entryname ? hash_get(hash, entryname) : (void *) hash);

  /* Get an revm object */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,ret, sizeof(revmobj_t), NULL);
  ret->parent   = ptr;
  ret->otype    = aspect_type_get_by_id((entryname ? hash->type : ASPECT_TYPE_HASH));
  ret->hname    = (hashname ? strdup(hashname) : NULL);
  ret->kname    = (entryname ? strdup(entryname) : NULL);
  ret->contype  = CONT_HASH;
  ret->perm     = 1;
  ret->immed    = 0;
  ret->get_obj  = revm_hash_getobj;
  ret->set_obj  = (void *) (entryname ? revm_long_setobj : NULL);

  /* Success */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}




/** 
 * @brief Parse a hash access 
 */
revmobj_t	*parse_list(char *param, char *fmt)
{
  unsigned int		size;
  char		index[ERESI_MEANING];
  list_t	*list;
  revmobj_t	*ret;
  char		*entryname;
  char		*listname;
  void		*ptr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  size = parse_lookup_varlist(param, fmt, index);
  if (size != 1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Parser handling failed", NULL);
  
  /* Get hash table and entry */
  entryname = strchr(index, ':');
  if (entryname)
    *entryname++ = 0x00;
  listname  = strdup(index);

  /* In case the hash table does not exist, create it empty */
  list = elist_find(listname);
  if (!list)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     list, sizeof(list_t), NULL);
      elist_init(list, listname, ASPECT_TYPE_UNKNOW);
    }

  /* Now deal with the entry */
  ptr = (entryname ? elist_get(list, entryname) : (void *) list);

  /* Get an revm object */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, ret, sizeof(revmobj_t), NULL);
  ret->parent   = ptr;
  ret->otype    = aspect_type_get_by_id((entryname ? list->type : ASPECT_TYPE_LIST));
  ret->hname    = (listname ? strdup(listname) : NULL);
  ret->kname    = (entryname ? strdup(entryname) : NULL);
  ret->contype  = CONT_LIST;
  ret->perm     = 1;
  ret->immed    = 0;
  ret->get_obj  = revm_hash_getobj; /* We can keep this one for lists too */
  ret->set_obj  = (void *) (entryname ? revm_long_setobj : NULL);

  /* Success */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}





/**
 * @brief Lookup a parameter with 3 fields, 3rd field beeing an index 
 * Used by GOT, CTORS, DTORS 
 */
revmobj_t		*parse_lookup3_index(char *param, char *fmt, unsigned int sep)
{
  revmL1_t		*l1;
  void			*robj;
  void			*o1;
  unsigned int			size;
  unsigned int			real_index;
  revmobj_t		*pobj;
  char			obj[ERESI_MEANING];
  char			L1field[ERESI_MEANING];
  char			index[ERESI_MEANING];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (sep != 1)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);

  real_index = 0;

  // Check if this handler is the correct one 
  size = parse_lookup_varlist(param, fmt, obj, L1field, index);
  if (size != 3)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parser handling failed", NULL);
  
  // Let's ask the hash table for the current working file
  robj = revm_lookup_file(obj);
  if (robj == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object", NULL);

  // Then, we ask the Level 1 object 
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L1 object", NULL);
  
  else if (l1->get_entptr == NULL || l1->get_obj == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Bad object path", NULL);
  
  pobj = revm_create_IMMED(ASPECT_TYPE_UNKNOW, 0, 0);
  pobj->immed = 0;

  // Lookup object 
  o1 = l1->get_obj(robj, &size);

  // Lookup index 
  real_index = revm_lookup_index(index);
  
  //printf("GOT real_index = " XFMT " unsigned: " UFMT " signed: " DFMT "\n", 
  // real_index, real_index, real_index);

  if (((int) real_index) < 0)
    {
      
      if (l1->get_obj_nam == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid L1 index", NULL);
      else
	{
	  
	  //printf("looking up get_obj_name in GOT ! \n");
	  
	  pobj->parent = l1->get_obj_nam(robj, index);
	  
	  /*
	  printf("GOT pobjparent = %p (with name = %s) \n", pobj->parent, index);
	  printf("GOT sect data  = %p (GOT name = %s) \n", 
		 ((elfshobj_t *) robj)->secthash[ELFSH_SECTION_GOT]->data,
		 ((elfshobj_t *) robj)->secthash[ELFSH_SECTION_GOT]->name);
	  */

	  if (pobj->parent == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "No entry by that name", NULL);
	}
    }

  // Do index sanity 
  if (pobj->parent == NULL)
    {
      if (size <= real_index)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Index too big", NULL);
      pobj->parent  = l1->get_entptr(o1, real_index);
    }

  // Finally we fill the intermediate object format for the guessed object 
  pobj->get_obj = (void *) l1->get_entval;
  pobj->set_obj = (void *) l1->set_entval;
  pobj->otype   = aspect_type_get_by_id(ASPECT_TYPE_CADDR);
  pobj          = revm_check_object(pobj);
  if (!pobj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid REVM object", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, pobj);
}




/**
 * Lookup a parameter with 3 fields, all fields beeing non indexed 
 * Only used by ELF header 'til now 
 */
revmobj_t		*parse_lookup3(char *param, char *fmt, unsigned int sep)
{
  revmL1_t		*l1;
  revmL2_t		*l2;
  void			*robj;
  revmobj_t		*pobj;

  char			obj[ERESI_MEANING];
  char			L1field[ERESI_MEANING];
  char			L2field[ERESI_MEANING];
  int			ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (sep != 2)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);

  // Check if this handler is the correct one 
  ret = parse_lookup_varlist(param, fmt, obj, L1field, L2field);
  if (ret != 3)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parser handling failed", NULL);

  // Let's ask the hash table for the current working file 
  robj = revm_lookup_file(obj);
  if (robj == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object",
		      NULL);

  // Then, we ask the Level 1 object 
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L1 object",
		      NULL);
  else if (l1->get_obj == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid object path",
		      NULL);

  // Then the Level 2 object 
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L2 object",
		      NULL);

  // Finally we fill the intermediate object format for the guessed object 
  pobj = revm_create_IMMED(ASPECT_TYPE_UNKNOW, 0, 0);
  pobj->immed = 0;
  pobj->get_obj = (void *) l2->get_obj;
  pobj->set_obj = (void *) l2->set_obj;
  pobj->otype   = aspect_type_get_by_id(l2->type);
  pobj->parent  = l1->get_obj(robj, NULL);
  pobj          = revm_check_object(pobj);

  if (!pobj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid REVM object", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, pobj);
}




/** 
 * Lookup a parameter with 4 fields, 3rd field beeing an index 
 * Used by SHT, PHT, SYMTAB, DYNSYM , Sections, .dynamic
 *
 * Here need to add 1.rel[name].{L2fields} lookup
 */
revmobj_t		*parse_lookup4(char *param, char *fmt, unsigned int sep)
{
  revmL1_t		*l1;
  revmL2_t		*l2;
  void			*robj;
  void			*o1;
  int		  	real_index;
  int			isversion;
  unsigned int			size;
  revmobj_t		*pobj;
  char			obj[ERESI_MEANING];
  char			L1field[ERESI_MEANING];
  char			L2field[ERESI_MEANING];
  char			index[ERESI_MEANING];
  char			offfield[ERESI_MEANING];
  char			sizelemfield[ERESI_MEANING];
  unsigned int			off;
  unsigned int			sizelem;
  int			ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* This handler has 3 possibles syntax */
  switch (sep)
    {
    case 4:
      ret = parse_lookup_varlist(param, fmt, obj, L1field, 
				 index, offfield, sizelemfield, L2field);
      sizelem = atoi(sizelemfield);
      off = atoi(offfield);
      break;
    case 3:
      ret = parse_lookup_varlist(param, fmt, obj, L1field, 
				 index, offfield, L2field);
      sizelem = 1;
      off = atoi(offfield);
      break;
    case 2:
      ret = parse_lookup_varlist(param, fmt, obj, L1field, index, L2field);
      sizelem = 1;
      off = 0;
      break;
    default:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
    }

  /* Quick test to see if we matched */
  if (ret - 2 != sep)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
  
  /* Let's ask the hash table for the current working file */
  robj = revm_lookup_file(obj);
  if (NULL == robj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown file object",
		   NULL);

  // Then, we ask the Level 1 object 
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown L1 object", 
		   NULL);
  else if (l1->get_entptr == NULL || l1->get_obj == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Incorrect object path",
		   NULL);

  // Then the Level 2 object 
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown L2 object", 
		   NULL);

  // Read object 
  o1 = l1->get_obj(robj, (void *) &size);
  if (o1 == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Cannot read object", 
		   NULL);
  
  pobj = revm_create_IMMED(ASPECT_TYPE_UNKNOW, 0, 0);
  pobj->immed = 0;

  // Do lookup by index or by name 
  real_index = (int) revm_lookup_index(index);
  
#if __DEBUG_LANG__
  printf("LOOKUP4 object index = %s, real_index = %u (signed = %d) \n", 
	 index, real_index, real_index);
#endif

  // Index error handling 
  if (real_index < 0)
    {
      if (l1->get_obj_nam == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid L1 index", NULL);
      else
	{
	  pobj->parent = l1->get_obj_nam(robj, index);
	  if (pobj->parent == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "No L1 index by this name", NULL);
	}
    }
  
  // Get L2 object by index (or type for dynamic) 
  if (pobj->parent == NULL)
    {

      if (!strcmp(L1field, "dynamic") && !revm_isnbr(index))
	real_index = elfsh_get_dynent_by_type(robj, o1, real_index);
      
      isversion = (!strcmp(L1field, "version") ||
		   !strcmp(L1field, "verdef") ||
		   !strcmp(L1field, "verneed"));
      
      if (!isversion && size <= real_index)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "L1 index too big", NULL);

      pobj->parent = l1->get_entptr(o1, real_index);

      if (isversion && pobj->parent == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown L2 object or Invalid L1 index", 
			  NULL);
    }

  // Finally we fill the intermediate object format for the guessed object 
  pobj->get_obj  = (void *) l2->get_obj;
  pobj->set_obj  = (void *) l2->set_obj;
  pobj->get_name = (void *) l2->get_name;
  pobj->set_name = (void *) l2->set_name;

  // The 2 next fields are used for 'raw' L2 of 'section' L1 
  pobj->get_data = (void *) l2->get_data;
  pobj->set_data = (void *) l2->set_data;

  pobj->otype    = aspect_type_get_by_id(l2->type);
  pobj->off      = off;
  pobj->sizelem  = sizelem;
  pobj->root     = robj;

  // Error checking 
  pobj = revm_check_object(pobj);
  if (!pobj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid REVM object", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, pobj);
}




/**
 * Lookup a parameter with 5 fields, 3rd and 5th field beeing indexes 
 * Used for Relocation tables and GOT tables 
 */
revmobj_t		*parse_lookup5_index(char *param, char *fmt, unsigned int sep)
{
  revmL1_t		*l1;
  revmL2_t		*l2;
  void			*robj;
  void			*o1;
  int			real_index;
  int			real_index2;
  unsigned int			size;
  int			isversion;
  revmobj_t		*pobj;
  elfshsect_t		*sect;
  char			obj[ERESI_MEANING];
  char			L1field[ERESI_MEANING];
  char			L2field[ERESI_MEANING];
  char			index[ERESI_MEANING];
  char			index2[ERESI_MEANING];
  int			ret;

#if 0
  char			logbuf[BUFSIZ];
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (sep != 2)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);

  // Check if this handler is the correct one 
  ret = parse_lookup_varlist(param, fmt, obj, L1field, index, index2, L2field);
  if (ret != 5)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parser handling failed", NULL);

  // Let's ask the hash table for the current working file 
  robj = revm_lookup_file(obj);
  if (robj == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object", 
		      NULL);

  // Then, we ask the Level 1 object 
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L1 object", NULL);
  else if (l1->get_entptr == NULL || l1->get_obj_idx == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "This object needs 2 indexes", NULL);

  // Then the Level 2 object 
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L2 object", NULL);
  else if (l2->get_obj == NULL || l2->set_obj == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Child object is invalid", NULL);

  // Not clean .. need to ve virtualized 
  isversion = (!strcmp(L1field, "version") ||
	       !strcmp(L1field, "verdef") ||
	       !strcmp(L1field, "verneed"));

  // Get indexes 
  real_index  = (int) revm_lookup_index(index);
  real_index2 = (int) revm_lookup_index(index2);

#if 0
 snprintf(logbuf, BUFSIZ - 1, 
	  "[DEBUG_MODEL] Lookup5_index : index(" UFMT ") rindex(" UFMT ") \n", 
	  real_index, real_index2);
 revm_output(logbuf);
#endif

  // Do index sanity 
  o1 = l1->get_obj_idx(robj, real_index, (unsigned int *) &size);
  if (!isversion && size <= real_index2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Second index too big", NULL);

  if (isversion && o1 == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid L1 index", NULL);    
  
  // printf("[DEBUG_RELOCS_IDX2] o1 = %p, o1->data = %p (%s) \n", 
  // o1, ((elfshsect_t*)o1)->data, ((elfshsect_t*)o1)->name);

  // 
  // This elfsh_get_reloc() returns a elfshsect_t *, 
  // and we need to grab the ->data pointer, to avoid rewriting a 
  // new handler (its not fair but it works .. ;) and its acceptable
  // since only relocation tables use this rule.
  
  sect = (elfshsect_t *) o1;
  o1 = elfsh_get_raw(sect);

  // Finally we fill the intermediate object format for the guessed object 
  pobj = revm_create_IMMED(ASPECT_TYPE_UNKNOW, 0, 0);
  pobj->immed = 0;
  pobj->get_obj = (void *) l2->get_obj;
  pobj->set_obj = (void *) l2->set_obj;
  pobj->otype   = aspect_type_get_by_id(l2->type);

  // ugly flag to put for dynamic Rel vs Rela information 
  elfsh_setrel(IS_REL(sect));
  pobj->parent  = l1->get_entptr(o1, real_index2);

  if (isversion && pobj->parent == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown L2 object or Invalid L2 index", 
		      NULL);

  // Error checking 
  pobj = revm_check_object(pobj);
  if (!pobj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid REVM object", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, pobj);
}
