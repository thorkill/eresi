/*
** grammar.c for elfsh
** 
** We dont use bison and have our own parser generator
**
** Started on  Sun Feb  9 22:57:58 2003 mayhem
*/
#include "revm.h"


/* Get a va_list of parameters */
static int	parse_lookup_varlist(char *param, char *fmt, ...)
{
  int		rc;
  va_list	arg_ptr;

  va_start(arg_ptr, fmt);
  rc = vsscanf(param, fmt, arg_ptr);
  va_end(arg_ptr);
  return (rc);
}


/* Parse a vector access */
revmobj_t	*parse_vector(char *param, char *fmt)
{
  u_int		size;
  char		index[ELFSH_MEANING];
  vector_t	*cur;
  int		dimnbr;
  unsigned int	*dims;
  revmobj_t	*ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  size = parse_lookup_varlist(param, fmt, index);
  if (size != 1)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Parser handling failed", NULL);
  if (!strchr(index, ':'))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Index parser failed", NULL);

  /* Grab the vector */
  dimnbr = vm_vectors_getdimnbr(index);
  dims = alloca(dimnbr * sizeof(unsigned int));
  vm_vectors_getdims(index, dims);
  cur = aspect_vector_get(index);

  /* Early sanity checks */
  if (!cur)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown requested vector", NULL);
  if (vm_vector_bad_dims(cur, dims, dimnbr))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Requested vector with bad dimensions", NULL);

  /* Get a pointer on the desired entry of the vector */
  XALLOC(ret, sizeof(revmobj_t), NULL);
  ret->parent   = aspect_vectors_selectptr(cur, dims);
  ret->type     = cur->type;
  ret->perm     = 1;
  ret->immed    = 0;
  ret->get_obj  = (void *) vm_generic_getobj;
  ret->set_obj  = (void *) vm_long_setobj;

  /* Success */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Parse a hash access */
revmobj_t	*parse_hash(char *param, char *fmt)
{
  u_int		size;
  char		index[ELFSH_MEANING];
  hash_t	*hash;
  revmobj_t	*ret;
  char		*entryname;
  void		*ptr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  size = parse_lookup_varlist(param, fmt, index);
  if (size != 1)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Parser handling failed", NULL);
  
  /* Get hash table and entry */
  entryname = strchr(index, ':');
  if (entryname)
    *entryname++ = 0x00;
  hash = hash_find(index);
  if (!hash)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unknown requested hash", NULL);
  if (entryname)
    {
      ptr = hash_get(hash, entryname);
      if (!ptr)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unknown requested hash entry", NULL);
    }
  else
    ptr = (void *) hash;

  /* Get an revm object */
  XALLOC(ret, sizeof(revmobj_t), NULL);
  ret->parent   = ptr;
  ret->type     = (entryname ? hash->type : REVM_TYPE_UNKNOW);
  ret->perm     = 1;
  ret->immed    = 0;
  ret->get_obj  = (void *) vm_generic_getobj;
  ret->set_obj  = (void *) vm_long_setobj;

  /* Success */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}





/* Lookup a parameter with 3 fields, 3rd field beeing an index 
** Used by GOT, CTORS, DTORS */
revmobj_t		*parse_lookup3_index(char *param, char *fmt)
{
  revmL1_t		*l1;
  void			*robj;
  void			*o1;
  u_int			size;
  elfsh_Addr		real_index = 0;
  revmobj_t		*pobj;
  char			obj[ELFSH_MEANING];
  char			L1field[ELFSH_MEANING];
  char			index[ELFSH_MEANING];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  // Check if this handler is the correct one 
  size = parse_lookup_varlist(param, fmt, obj, L1field, index);
  if (size != 3)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parser handling failed", NULL);
  
  // Let's ask the hash table for the current working file
  robj = vm_lookup_file(obj);
  if (robj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object", NULL);

  // Then, we ask the Level 1 object 
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L1 object", NULL);
  
  else if (l1->get_entptr == NULL || l1->get_obj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Bad object path", NULL);
  
  pobj = vm_create_IMMED(REVM_TYPE_UNKNOW, 0, 0);
  pobj->immed = 0;

  // Lookup object 
  o1 = l1->get_obj(robj, &size);

  // Lookup index 
  real_index = vm_lookup_index(index);
  
  printf("GOT real_index = " XFMT " unsigned: " UFMT " signed: " DFMT "\n", 
	 real_index, real_index, real_index);

  if (((int) real_index) < 0)
    {
      
      if (l1->get_obj_nam == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid L1 index", NULL);
      else
	{
	  
	  printf("looking up get_obj_name in GOT ! \n");

	  pobj->parent = l1->get_obj_nam(robj, index);

	  printf("GOT pobjparent = %p (with name = %s) \n", pobj->parent, index);
	  printf("GOT sect data  = %p (GOT name = %s) \n", 
		 ((elfshobj_t *) robj)->secthash[ELFSH_SECTION_GOT]->data,
		 ((elfshobj_t *) robj)->secthash[ELFSH_SECTION_GOT]->name);
	  
	  if (pobj->parent == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "No entry by that name", NULL);
	}
    }

  // Do index sanity 
  if (pobj->parent == NULL)
    {
      if (size <= real_index)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Index too big", NULL);
      pobj->parent  = l1->get_entptr(o1, real_index);
    }

  // Finally we fill the intermediate object format for the guessed object 
  pobj->get_obj = (void *) l1->get_entval;
  pobj->set_obj = (void *) l1->set_entval;
  pobj->type    = REVM_TYPE_LONG;
  pobj          = vm_check_object(pobj);
  if (!pobj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid REVM object", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, pobj);
}




/* Lookup a parameter with 3 fields, all fields beeing non indexed 
** Only used by ELF header 'til now */
revmobj_t		*parse_lookup3(char *param, char *fmt)
{
  revmL1_t		*l1;
  revmL2_t		*l2;
  void			*robj;
  revmobj_t		*pobj;

  char			obj[ELFSH_MEANING];
  char			L1field[ELFSH_MEANING];
  char			L2field[ELFSH_MEANING];
  int			ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  // Check if this handler is the correct one 
  ret = parse_lookup_varlist(param, fmt, obj, L1field, L2field);
  if (ret != 3)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parser handling failed", NULL);

  // Let's ask the hash table for the current working file 
  robj = vm_lookup_file(obj);
  if (robj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object",
		      NULL);

  // Then, we ask the Level 1 object 
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L1 object",
		      NULL);
  else if (l1->get_obj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid object path",
		      NULL);

  // Then the Level 2 object 
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L2 object",
		      NULL);

  // Finally we fill the intermediate object format for the guessed object 
  pobj = vm_create_IMMED(REVM_TYPE_UNKNOW, 0, 0);
  pobj->immed = 0;
  pobj->get_obj = (void *) l2->get_obj;
  pobj->set_obj = (void *) l2->set_obj;
  pobj->type    = l2->type;
  pobj->parent  = l1->get_obj(robj, NULL);
  pobj          = vm_check_object(pobj);

  if (!pobj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid REVM object", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, pobj);
}




/* 
** Lookup a parameter with 4 fields, 3rd field beeing an index 
** Used by SHT, PHT, SYMTAB, DYNSYM , Sections, .dynamic
**
** Here need to add 1.rel[name].{L2fields} lookup
*/
revmobj_t		*parse_lookup4(char *param, char *fmt)
{
  revmL1_t		*l1;
  revmL2_t		*l2;
  void			*robj;
  void			*o1;
  int		  	real_index;
  int			isversion;
  u_int			size;
  revmobj_t		*pobj;

  char			obj[ELFSH_MEANING];
  char			L1field[ELFSH_MEANING];
  char			L2field[ELFSH_MEANING];
  char			index[ELFSH_MEANING];
  u_int			off;
  u_int			sizelem;
  int			ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  // Check if this handler is the correct one 
  // This handler has 3 different possible parsing rules 
  ret = parse_lookup_varlist(param, fmt, obj, L1field, 
			  index, &off, &sizelem, L2field);
  if (ret != 6)
    {
      ret = parse_lookup_varlist(param, fmt, obj, L1field, index, &off, L2field);
      if (ret != 5)
	{
	  ret = parse_lookup_varlist(param, fmt, obj, L1field, index, L2field);
	  if (ret != 4)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Parser handling failed", NULL);
	  sizelem = 1;
	  off = 0;
	}
      else
	sizelem = 1;
    }

  // Let's ask the hash table for the current working file 
  robj = vm_lookup_file(obj);
  if (NULL == robj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown file object",
		   NULL);

  // Then, we ask the Level 1 object 
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown L1 object", 
		   NULL);
  else if (l1->get_entptr == NULL || l1->get_obj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Incorrect object path",
		   NULL);

  // Then the Level 2 object 
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown L2 object", 
		   NULL);

  // Read object 
  o1 = l1->get_obj(robj, (void *) &size);
  if (o1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Cannot read object", 
		   NULL);
  
  pobj = vm_create_IMMED(REVM_TYPE_UNKNOW, 0, 0);
  pobj->immed = 0;

  // Do lookup by index or by name 
  real_index = vm_lookup_index(index);
  
#if __DEBUG_LANG__
  printf("LOOKUP4 object index = %s, real_index = %u (signed = %d) \n", 
	 index, real_index, real_index);
#endif

  // Index error handling 
  if (((int) real_index) < 0)
    {
      if (l1->get_obj_nam == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid L1 index", NULL);
      else
	{
	  pobj->parent = l1->get_obj_nam(robj, index);
	  if (pobj->parent == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "No L1 index by this name", NULL);
	}
    }
  
  // Get L2 object by index (or type for dynamic) 
  if (pobj->parent == NULL)
    {

      if (!strcmp(L1field, "dynamic") && !vm_isnbr(index))
	real_index = elfsh_get_dynent_by_type(robj, o1, real_index);
      
      isversion = (!strcmp(L1field, "version") ||
		   !strcmp(L1field, "verdef") ||
		   !strcmp(L1field, "verneed"));
      
      if (!isversion && size <= real_index)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "L1 index too big", 
	NULL);

      pobj->parent = l1->get_entptr(o1, real_index);

      if (isversion && pobj->parent == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
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

  pobj->type     = l2->type;
  pobj->off      = off;
  pobj->sizelem  = sizelem;
  pobj->root     = robj;

  // Error checking 
  pobj = vm_check_object(pobj);
  if (!pobj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid REVM object", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, pobj);
}




/* Lookup a parameter with 5 fields, 3rd and 5th field beeing indexes 
** Used for Relocation tables and GOT tables */
revmobj_t		*parse_lookup5_index(char *param, char *fmt)
{
  revmL1_t		*l1;
  revmL2_t		*l2;
  void			*robj;
  void			*o1;
  volatile elfsh_Addr  	real_index;
  volatile elfsh_Addr  	real_index2;
  volatile u_int	size;
  int			isversion;
  revmobj_t		*pobj;
  elfshsect_t		*sect;

  char			obj[ELFSH_MEANING];
  char			L1field[ELFSH_MEANING];
  char			L2field[ELFSH_MEANING];
  char			index[ELFSH_MEANING];
  char			index2[ELFSH_MEANING];
  int			ret;

#if 0
  char			logbuf[BUFSIZ];
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  // Check if this handler is the correct one 
  ret = parse_lookup_varlist(param, fmt, obj, L1field, index, index2, L2field);
  if (ret != 5)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parser handling failed", NULL);

  // Let's ask the hash table for the current working file 
  robj = vm_lookup_file(obj);
  if (robj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object", 
		      NULL);

  // Then, we ask the Level 1 object 
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L1 object", NULL);
  else if (l1->get_entptr == NULL || l1->get_obj_idx == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "This object needs 2 indexes", NULL);

  // Then the Level 2 object 
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L2 object", NULL);
  else if (l2->get_obj == NULL || l2->set_obj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Child object is invalid", NULL);

  // Not clean .. need to ve virtualized 
  isversion = (!strcmp(L1field, "version") ||
	       !strcmp(L1field, "verdef") ||
	       !strcmp(L1field, "verneed"));

  // Get indexes 
  real_index = vm_lookup_index(index);
  real_index2 = vm_lookup_index(index2);

#if 0
 snprintf(logbuf, BUFSIZ - 1, 
	  "[DEBUG_MODEL] Lookup5_index : index(" UFMT ") rindex(" UFMT ") \n", 
	  real_index, real_index2);
 vm_output(logbuf);
#endif

  // Do index sanity 
  o1 = l1->get_obj_idx(robj, real_index, (u_int *) &size);
  if (!isversion && size <= real_index2)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Second index too big", NULL);

  if (isversion && o1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
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
  pobj = vm_create_IMMED(REVM_TYPE_UNKNOW, 0, 0);
  pobj->immed = 0;
  pobj->get_obj = (void *) l2->get_obj;
  pobj->set_obj = (void *) l2->set_obj;
  pobj->type    = l2->type;

  // ugly flag to put for dynamic Rel vs Rela information 
  elfsh_setrel(IS_REL(sect));
  pobj->parent  = l1->get_entptr(o1, real_index2);

  if (isversion && pobj->parent == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown L2 object or Invalid L2 index", 
		      NULL);

  // Error checking 
  pobj = vm_check_object(pobj);
  if (!pobj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid REVM object", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, pobj);
}



/*
** Parse the parameter and fill the revmobj_t
** Change ELFSH_FIELD_SEP in elfsh/elfsh/include/revm.h
*/
revmobj_t		*vm_lookup_param(char *param, u_int m)
{
  revmobj_t		*(*funcptr)(char *param, char *fmt);
  void			*ret;
  char			**keys;
  int			keynbr;
  char			*parm;
  unsigned int		sepnbr;
  unsigned int		index;
  revmobj_t		*res;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Find the number of fields in param */
  sepnbr = 0;
  parm   = param;
  do {    
    parm = strchr(parm, REVM_SEP[0]);
    if (parm)
      {
	sepnbr++;
	parm++;
      }
  } while (parm && *parm);
  
  /* Find the correct parsing handler */
  keys = hash_get_keys(&parser_hash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      funcptr = hash_get(&parser_hash, keys[index]);
      ret = funcptr(param, keys[index]);
      if (!ret)
	continue;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);  
    }

  /* If still not found, try manually inserted types */
  ret = vm_revmobj_lookup(param);
  if (ret)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);  

  /* If no good syntax is available, print error if we are not in probe mode */
  if (m)
    {
      res = vm_lookup_immed(param);
      if (!res || (m == 2 && res->immed))
	vm_badparam(param);
      else
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (res));
    }

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to resolve object", NULL);
}
