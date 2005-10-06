/*
** grammar.c for elfsh
** 
** Need to use bison instead of vm_lookup_param()
** but bison is not that light, particulary when
** using an embedded elfsh ;>
**
** Started on  Sun Feb  9 22:57:58 2003 mayhem
*/
#include "elfsh.h"



/* 
** Lookup a parameter with 3 fields, 3rd field beeing an index 
** Used by GOT, CTORS, DTORS 
*/
static elfshpath_t	*lookup3_index(char *obj, char *L1field, char *index)
				      
{
  elfshL1_t		*l1;
  void			*robj;
  void			*o1;
  u_int			size;
  elfsh_Addr		real_index = 0;
  elfshpath_t		*pobj;

#if __DEBUG_MODEL__
  char			logbuf[BUFSIZ];
  
  snprintf(logbuf, BUFSIZ - 1, "[DEBUG_MODEL] Lookup3 index\n");
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Let's ask the hash table for the current working file */
  robj = vm_lookup_file(obj);
  if (robj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object", NULL);

  /* Then, we ask the Level 1 object */
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L1 object", NULL);
  
  else if (l1->get_entptr == NULL || l1->get_obj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Bad object path", NULL);
  
  pobj = vm_create_IMMED(ELFSH_OBJUNK, 0, 0);
  pobj->immed = 0;

  /* Lookup object */
  o1 = l1->get_obj(robj, &size);

  /* Lookup index */
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

  /* Do index sanity */
  if (pobj->parent == NULL)
    {
      if (size <= real_index)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Index too big", NULL);
      pobj->parent  = l1->get_entptr(o1, real_index);
    }

  /* Finally we fill the intermediate object format for the guessed object */
  pobj->get_obj = (void *) l1->get_entval;
  pobj->set_obj = (void *) l1->set_entval;
  pobj->type    = ELFSH_OBJLONG;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, vm_check_object(pobj));
}




/* 
** Lookup a parameter with 3 fields, all fields beeing non indexed 
** Only used by ELF header 'til now
*/
static elfshpath_t	*lookup3(char *obj, char *L1field, char *L2field)
{
  elfshL1_t		*l1;
  elfshL2_t		*l2;
  void			*robj;
  elfshpath_t		*pobj;

#if __DEBUG_MODEL__
  char			logbuf[BUFSIZ];
  
  snprintf(logbuf, BUFSIZ, "[DEBUG_MODEL] Lookup3 \n");
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Let's ask the hash table for the current working file */
  robj = vm_lookup_file(obj);
  if (robj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object",
		      NULL);

  /* Then, we ask the Level 1 object */
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L1 object",
		      NULL);
  else if (l1->get_obj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid object path",
		      NULL);

  /* Then the Level 2 object */
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L2 object",
		      NULL);

  /* Finally we fill the intermediate object format for the guessed object */
  pobj = vm_create_IMMED(ELFSH_OBJUNK, 0, 0);
  pobj->immed = 0;
  pobj->get_obj = (void *) l2->get_obj;
  pobj->set_obj = (void *) l2->set_obj;
  pobj->type    = l2->type;
  pobj->parent  = l1->get_obj(robj, NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     vm_check_object(pobj));
}








/* Just a test */
int		vm_get_dynent_by_type(elfshobj_t	*robj, 
				      elfsh_Dyn		*data,
				      elfsh_Word	real_index)
{
  elfsh_Dyn	*ent;
  int		idx;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ent = elfsh_get_dynamic_entry_by_type(robj, real_index);
  idx = (int) ((char *) ent - (char *) data);
  idx = idx / sizeof(elfsh_Dyn);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     idx);
}







/* 
** Lookup a parameter with 4 fields, 3rd field beeing an index 
** Used by SHT, PHT, SYMTAB, DYNSYM , Sections, .dynamic
**
** Here need to add 1.rel[name].{L2fields} lookup
*/
static elfshpath_t	*lookup4(char *obj, char *L1field, 
				 char *index, char *L2field, 
				 u_int off, u_int sizelem)
{
  elfshL1_t		*l1;
  elfshL2_t		*l2;
  void			*robj;
  void			*o1;
  int		  	real_index, isversion;
  u_int			size;
  elfshpath_t		*pobj;

#if __DEBUG_MODEL__
  char			logbuf[BUFSIZ];
#endif

#if __DEBUG_MODEL__
 snprintf(logbuf, BUFSIZ - 1, "[DEBUG_MODEL] Lookup4 : off = %u \n", off);
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Let's ask the hash table for the current working file */
  robj = vm_lookup_file(obj);
  if (NULL == robj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown file object",
		   NULL);

  /* Then, we ask the Level 1 object */
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown L1 object", 
		   NULL);
  else if (l1->get_entptr == NULL || l1->get_obj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Incorrect object path",
		   NULL);

  /* Then the Level 2 object */
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown L2 object", 
		   NULL);

  /* Read object */
  o1 = l1->get_obj(robj, (void *) &size);
  if (o1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Cannot read object", 
		   NULL);
  
  pobj = vm_create_IMMED(ELFSH_OBJUNK, 0, 0);
  pobj->immed = 0;

  /* Do lookup by index or by name */
  real_index = vm_lookup_index(index);
  
#if __DEBUG_MODEL__
  printf("SYM index = %s, real_index = %u (signed = %d) \n", 
	 index, real_index, real_index);
#endif

  /* Index error handling */
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
  
  /* Get L2 object by index (or type for dynamic) */
  if (pobj->parent == NULL)
    {

      if (!strcmp(L1field, "dynamic") && !vm_isnbr(index))
	real_index = vm_get_dynent_by_type(robj, o1, real_index);
      
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

  /* Finally we fill the intermediate object format for the guessed object */
  pobj->get_obj  = (void *) l2->get_obj;
  pobj->set_obj  = (void *) l2->set_obj;
  pobj->get_name = l2->get_name;
  pobj->set_name = l2->set_name;

  /* The 2 next fields are used for 'raw' L2 of 'section' L1 */
  pobj->get_data = l2->get_data;
  pobj->set_data = l2->set_data;

  pobj->type     = l2->type;
  pobj->off      = off;
  pobj->sizelem  = sizelem;
  pobj->root     = robj;

  /* Error checking */
  //snprintf(logbuf, BUFSIZ, "calling checking pobj = %p \n", pobj);
  //vm_output(logbuf);
  pobj = vm_check_object(pobj);
  //snprintf(logbuf, BUFSIZ, "returning pobj = %p \n", pobj);
  //vm_output(logbuf);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, pobj);
}


/* 
** Lookup a parameter with 5 fields, 3rd and 5th field beeing indexes 
** Used for Relocation tables and GOT tables.
*/
static elfshpath_t	*lookup5_index(char *obj, char *L1field, char *index, 
				       char *index2, char *L2field)
{
  elfshL1_t		*l1;
  elfshL2_t		*l2;
  void			*robj;
  void			*o1;
  volatile elfsh_Addr  	real_index;
  volatile elfsh_Addr  	real_index2;
  volatile u_int	size;
  int			isversion;
  elfshpath_t		*pobj;
  elfshsect_t		*sect;
#if 0
  char			logbuf[BUFSIZ];
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  real_index = vm_lookup_index(index);
  real_index2 = vm_lookup_index(index2);

#if 0
 snprintf(logbuf, BUFSIZ - 1, 
	  "[DEBUG_MODEL] Lookup5_index : index(" UFMT ") rindex(" UFMT ") \n", 
	  real_index, real_index2);
 vm_output(logbuf);
#endif

  /* Let's ask the hash table for the current working file */
  robj = vm_lookup_file(obj);
  if (robj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object", 
		      NULL);

  /* Then, we ask the Level 1 object */
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L1 object", NULL);
  else if (l1->get_entptr == NULL || l1->get_obj_idx == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "This object needs 2 indexes", NULL);

  /* Then the Level 2 object */
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested L2 object", NULL);
  else if (l2->get_obj == NULL || l2->set_obj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Child object is invalid", NULL);

  /* Not clean .. need to ve virtualized */
  isversion = (!strcmp(L1field, "version") ||
	       !strcmp(L1field, "verdef") ||
	       !strcmp(L1field, "verneed"));

  /* Do index sanity */
  o1 = l1->get_obj_idx(robj, real_index, (u_int *) &size);
  if (!isversion && size <= real_index2)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Second index too big", NULL);

  if (isversion && o1 == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid L1 index", NULL);    
  
  // printf("[DEBUG_RELOCS_IDX2] o1 = %p, o1->data = %p (%s) \n", 
  // o1, ((elfshsect_t*)o1)->data, ((elfshsect_t*)o1)->name);

  /* 
  ** This elfsh_get_reloc() returns a elfshsect_t *, 
  ** and we need to grab the ->data pointer, to avoid rewriting a 
  ** new handler (its not fair but it works .. ;) and its acceptable
  ** since only relocation tables use this rule.
  */
  sect = (elfshsect_t *) o1;
  o1 = elfsh_get_raw(sect);

  /* Finally we fill the intermediate object format for the guessed object */
  pobj = vm_create_IMMED(ELFSH_OBJUNK, 0, 0);
  pobj->immed = 0;
  pobj->get_obj = (void *) l2->get_obj;
  pobj->set_obj = (void *) l2->set_obj;
  pobj->type    = l2->type;

  /* ugly flag to put for dynamic Rel vs Rela information */
  elfsh_setrel(IS_REL(sect));
  pobj->parent  = l1->get_entptr(o1, real_index2);

  if (isversion && pobj->parent == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown L2 object or Invalid L2 index", 
		      NULL);

  //printf("[DEBUG_RELOCS_IDX2-AFTER] entptr = %p \n",
  // pobj->parent);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     vm_check_object(pobj));
}




/*
** Parse the parameter and fill the elfshpath_t
** Change ELFSH_FIELD_SEP in elfsh/elfsh/include/elfsh.h
*/
elfshpath_t		*vm_lookup_param(char *param, u_int m)
{
  char			obj[ELFSH_MEANING];
  char			field1[ELFSH_MEANING];
  char			field2[ELFSH_MEANING];
  char			index[ELFSH_MEANING];
  char			index2[ELFSH_MEANING];
  char			off[ELFSH_MEANING];
  char			sizelem[ELFSH_MEANING];
  elfshpath_t		*res;
  int			ret;

#if __DEBUG_MODEL__
  char			logbuf[BUFSIZ];

 snprintf(logbuf, BUFSIZ - 1, "[DEBUG_MODEL] Looking up Parameter %s \n", param);
 vm_output(logbuf);
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* RULE1 : 2 levels, with one index per level : 5 field */
  ret = sscanf(param, 
	       "%41[^"ELFSH_FIELD_SEP"]"
	       ELFSH_FIELD_SEP
	       "%41[^[][%41[^]]][%41[^]]]"
	       ELFSH_FIELD_SEP
	       "%41s",
	       obj, field1, index, index2, field2);
  if (ret == 5)
    {
      res = lookup5_index(obj, field1, index, index2, field2);
      //if (res)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (res));
    }

  /* RULE2: 2 levels with index:offset%elemsize at level 1 : 4 fields */
  ret = sscanf(param, 
	       "%41[^"ELFSH_FIELD_SEP"]"
	       ELFSH_FIELD_SEP
	       "%41[^[][%41[^:]:%41[^%%]%%%41[^]]]"
	       ELFSH_FIELD_SEP
	       "%41s",
	       obj, field1, index, off, sizelem, field2);
  if (ret == 6)
    {
      res = lookup4(obj, field1, index, field2, atoi(off), atoi(sizelem));
      //if (res)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (res));
    }

  /* RULE2 bis: 2 levels with index:offset at level 1 : 5 fields */
  ret = sscanf(param, 
	       "%41[^"ELFSH_FIELD_SEP"]"
	       ELFSH_FIELD_SEP
	       "%41[^[][%41[^:]:%41[^]]]"
	       ELFSH_FIELD_SEP
	       "%41s",
	       obj, field1, index, off, field2);
  if (ret == 5)
    {
      res = lookup4(obj, field1, index, field2, atoi(off), 1);
      //if (res)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (res));
    }

  /* RULE2 ter: 2 levels with index at level 1 : 4 fields */
  ret = sscanf(param, 
	       "%41[^"ELFSH_FIELD_SEP"]"
	       ELFSH_FIELD_SEP
	       "%41[^[][%41[^]]]"
	       ELFSH_FIELD_SEP
	       "%41s",
	       obj, field1, index, field2);
  if (ret == 4)
    {
      res = lookup4(obj, field1, index, field2, 0, 1);
      //if (res)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (res));
    }

  /* RULE3: Only 1 level with 1 index : 3 fields [got/ctors/dtors] */
  ret = sscanf(param, 
	       "%41[^"ELFSH_FIELD_SEP"]"
	       ELFSH_FIELD_SEP
	       "%41[^[][%41[^]]]", 
	       obj, field1, index);
  if (ret == 3)
    {
      res = lookup3_index(obj, field1, index);
      //if (res)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (res));
    }
  
  /* RULE4: 2 level but no index : 3 fields */
  ret = sscanf(param, 
	       "%41[^"ELFSH_FIELD_SEP"]"
	       ELFSH_FIELD_SEP
	       "%41[^"ELFSH_FIELD_SEP"]"
	       ELFSH_FIELD_SEP
	       "%41s", 
	       obj, field1, field2);
  if (ret == 3)
    {
      res = lookup3(obj, field1, field2);
      //if (res)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (res));
    }

  /* RULE5: Try immediate value */
  /* If no good syntax is available, print error if we are not in probe mode */
  /* Patched 30-08-2004 for catching more errors in the set command */
  if (m)
    {
      res = vm_lookup_immed(param);
      if (res)
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (res));
      vm_badparam(param);
    }
  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup object", (NULL));
}
