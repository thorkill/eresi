/*
** grammar.c for elfsh
** 
** Need to use bison instead of vm_lookup_param()
** 
** Started on  Sun Feb  9 22:57:58 2003 mayhem
** Last update Tue May 27 20:07:36 2003 mayhem
*/
#include "elfsh.h"



/* Get immediate value */
static int	        lookup_immed(char *param, elfshpath_t *obj)
{
  Elf32_Sym		*sym;
  elfshconst_t		*actual;
  char			eol;
  int			ret;
  char			lbuf[4096];
  char			isptr;

#if __DEBUG_MODEL__
  printf("[DEBUG_MODEL] Lookup immed : PARAM [ %s ] \n", param);
#endif

  /* Support pointers */
  if (param != NULL && *param == '$')
    {
      isptr = 1;
      param++;
    }
  else
    isptr = 0;

  /* Lookup .symtab */
  sym = elfsh_get_symbol_by_name(world.current, param);
  if (sym != NULL)
    {
      obj->immed_val.ent = sym->st_value;
      obj->type = ELFSH_OBJINT;
      goto good;
    }

  /* Lookup .dynsym */
  sym = elfsh_get_dynsymbol_by_name(world.current, param);
  if (sym != NULL)
    {
      obj->immed_val.ent = sym->st_value;
      obj->type = ELFSH_OBJINT;
      goto good;
    }

  /* Lookup a constant */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    {
      obj->immed_val.ent = actual->val;
      obj->type = ELFSH_OBJINT;
      goto good;
    }

  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, "0x%lX%c", &obj->immed_val.ent, &eol);
  if (ret == 1)
    {
      obj->type = ELFSH_OBJINT;
      goto good;
    }

  /* Lookup decimal numeric value */
  ret = sscanf(param, "%ld%c", &obj->immed_val.ent, &eol);
  if (ret == 1)
    {
      obj->type = ELFSH_OBJINT;
      goto good;
    }

  /* Lookup a supplied string */
  bzero(lbuf, sizeof(lbuf));
  ret = sscanf(param, "%[^\n]4095%c", lbuf, &eol);
  if (ret == 1)
    {
      obj->type = ELFSH_OBJSTR;
      obj->immed_val.str = strdup(lbuf);
      obj->size = strlen(lbuf);
      goto good;
    }

  /* No match */
  return (-1);

  /* Match */
 good:
  obj->immed = 1;
  if(obj->type == ELFSH_OBJSTR)
    vm_filter_zero(obj);
  return (0);
}


/* 
** Lookup a parameter with 3 fields, 3rd field beeing an index 
** Used by GOT, CTORS, DTORS 
*/
static int	        lookup3_index(elfshpath_t *pobj, char *obj, 
				      char *L1field, char *index)
{
  elfshL1_t		*l1;
  void			*robj;
  u_int			idx;
  void			*o1;
  u_int			size;
  u_int			real_index = 0;
  
#if __DEBUG_MODEL__
  printf("[DEBUG_MODEL] Lookup3 index\n");
#endif

  /* Let's ask the hash table for the current working file */
  idx = atoi(obj);
  robj = (idx ? vm_getfile(idx) : hash_get(&file_hash, obj));
  if (robj == NULL)
    ELFSH_SETERROR("[elfsh:lookup3idx] Cannot find requested file object\n", -1);

  /* Then, we ask the Level 1 object */
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_SETERROR("[elfsh:lookup3idx] Cannot find requested L1 object\n", -1);
  else if (l1->get_entptr == NULL || l1->get_obj == NULL)
    ELFSH_SETERROR("[elfsh:lookup3idx] Bad object path\n", -1);

  /* Lookup object */
  o1 = l1->get_obj(robj, &size);
  if (vm_isnbr(index))
    real_index = atoi(index);
  else if (l1->get_obj_nam == NULL)
    ELFSH_SETERROR("[elfsh:lookup3idx] Cannot lookup by name\n", -1);
  else
    {
      pobj->parent = l1->get_obj_nam(robj, index);
      if (pobj->parent == NULL)
	ELFSH_SETERROR("[elfsh:lookup3idx] No entry by that name\n", -1);
    }

  /* Do index sanity */
  if (pobj->parent == NULL)
    {
      if (size <= real_index)
	ELFSH_SETERROR("[elfsh:lookup3idx] Index too big\n", -1);
      pobj->parent  = l1->get_entptr(o1, real_index);
    }

  /* Finally we fill the intermediate object format for the guessed object */
  pobj->get_obj = (void *) l1->get_entval;
  pobj->set_obj = (void *) l1->set_entval;
  pobj->type    = ELFSH_OBJINT;

  return (vm_check_object(pobj));
}




/* 
** Lookup a parameter with 3 fields, all fields beeing non indexed 
** Only used by ELF header 'til now
*/
static int		lookup3(elfshpath_t *pobj, char *obj, 
				char *L1field, char *L2field)
{
  elfshL1_t		*l1;
  elfshL2_t		*l2;
  void			*robj;
  u_int			idx;

#if __DEBUG_MODEL__
  printf("[DEBUG_MODEL] Lookup3 \n");
#endif

  /* Let's ask the hash table for the current working file */
  idx = atoi(obj);
  robj = (idx ? vm_getfile(idx) : hash_get(&file_hash, obj));
  if (robj == NULL)
    ELFSH_SETERROR("[elfsh:lookup3] Cannot find requested file object\n", -1);

  /* Then, we ask the Level 1 object */
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_SETERROR("[elfsh:lookup3] Cannot find requested L1 object\n", -1);
  else if (l1->get_obj == NULL)
    ELFSH_SETERROR("[elfsh:lookup3] Invalid object path\n", -1);

  /* Then the Level 2 object */
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    ELFSH_SETERROR("[elfsh:lookup3] Cannot find requested L2 object\n", -1);

  /* Finally we fill the intermediate object format for the guessed object */
  pobj->get_obj = (void *) l2->get_obj;
  pobj->set_obj = (void *) l2->set_obj;
  pobj->type    = l2->type;
  pobj->parent  = l1->get_obj(robj, NULL);
  return (vm_check_object(pobj));
}


/* 
** Lookup a parameter with 4 fields, 3rd field beeing an index 
** Used by SHT, PHT, SYMTAB, DYNSYM , Sections
*/
static int		lookup4(elfshpath_t *pobj,
				char *obj, char *L1field, 
				char *index, char *L2field, 
				u_int off, u_int sizelem)
{
  elfshL1_t		*l1;
  elfshL2_t		*l2;
  void			*robj;
  void			*o1;
  volatile int		real_index;
  volatile u_int	idx;
  volatile u_int	size;

#if __DEBUG_MODEL__
  printf("[DEBUG_MODEL] Lookup4 : off = %u \n", off);
#endif

  /* Let's ask the hash table for the current working file */
  idx = atoi(obj);
  robj = (idx ? vm_getfile(idx) : hash_get(&file_hash, obj));
  if (NULL == robj)
    ELFSH_SETERROR("[elfsh:lookup4] Unknown file object\n", -1);

  /* Then, we ask the Level 1 object */
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_SETERROR("[elfsh:lookup4] Unknown L1 object\n", -1);
  else if (l1->get_entptr == NULL || l1->get_obj == NULL)
    ELFSH_SETERROR("[elfsh:lookup4] Incorrect object path\n", -1);

  /* Then the Level 2 object */
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    ELFSH_SETERROR("[elfsh:lookup4] Unknown L2 object\n", -1);

  /* Read object */
  o1 = l1->get_obj(robj, (void *) &size);
  if (o1 == NULL)
    ELFSH_SETERROR("[elfsh:lookup4] Cannot read object\n", -1);
  
  /* Do lookup by index or by name */
  if (vm_isnbr(index))
    real_index = atoi(index);
  else if (l1->get_obj_nam == NULL)
    ELFSH_SETERROR("[elfsh:lookup4] Cannot lookup by name\n", -1);
  else
    {
      pobj->parent = l1->get_obj_nam(robj, index);
      if (pobj->parent == NULL)
	ELFSH_SETERROR("[elfsh:lookup4] No object by this name\n", -1);
    }
  if (pobj->parent == NULL)
    {
      if (size <= real_index)
	ELFSH_SETERROR("[elfsh:lookup4] L1 index too big\n", -1);
      pobj->parent   = l1->get_entptr(o1, real_index);
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
  return (vm_check_object(pobj));
}


/* 
** Lookup a parameter with 5 fields, 3rd and 5th field beeing indexes 
** Only used for Relocation tables 'til now .
*/
static int		lookup5_index(elfshpath_t *pobj, char *obj, 
				      char *L1field, char *index, 
				      char *index2, char *L2field)
{
  elfshL1_t		*l1;
  elfshL2_t		*l2;
  void			*robj;
  void			*o1;
  volatile int		real_index;
  volatile int		real_index2;
  volatile u_int	idx;
  volatile u_int	size;

  real_index = atoi(index);
  real_index2 = atoi(index2);

#if __DEBUG_MODEL__
  printf("[DEBUG_MODEL] Lookup5_index : index(%u) rindex(%u) \n", 
	 real_index, real_index2);
#endif

  /* Let's ask the hash table for the current working file */
  idx = atoi(obj);
  robj = (idx ? vm_getfile(idx) : hash_get(&file_hash, obj));
  if (robj == NULL)
    ELFSH_SETERROR("[elfsh:lookup5idx] Cannot find requested file object\n", -1);

  /* Then, we ask the Level 1 object */
  l1 = hash_get(&L1_hash, L1field);
  if (l1 == NULL)
    ELFSH_SETERROR("[elfsh:lookup5idx] Cannot find requested L1 object\n", -1);
  else if (l1->get_entptr == NULL || l1->get_obj_idx == NULL)
    ELFSH_SETERROR("[elfsh:lookup5idx] This object needs 2 indexes\n", -1);

  /* Then the Level 2 object */
  l2 = hash_get(l1->l2list, L2field);
  if (l2 == NULL)
    ELFSH_SETERROR("[elfsh:lookup5idx] Cannot find requested L2 object\n", -1);
  else if (l2->get_obj == NULL || l2->set_obj == NULL)
    ELFSH_SETERROR("[elfsh:lookup5idx] Child object is invalid\n", -1);

  /* Do index sanity */
  o1 = l1->get_obj_idx(robj, real_index, (u_int *) &size);
  if (size <= real_index2)
    ELFSH_SETERROR("[elfsh:lookup5idx] Second index too big\n", -1);
  
  /* 
  ** This elfsh_get_reloc() returns a elfshsect_t *, 
  ** and we need to grab the ->data pointer, to avoid rewriting a 
  ** new handler (its not fair but it works .. ;) and its acceptable
  ** since only relocation tables use this rule.
  */
  o1 = ((elfshsect_t *) o1)->data;

  /* Finally we fill the intermediate object format for the guessed object */
  pobj->get_obj = (void *) l2->get_obj;
  pobj->set_obj = (void *) l2->set_obj;
  pobj->type    = l2->type;
  pobj->parent  = l1->get_entptr(o1, real_index2);

  return (vm_check_object(pobj));
}




/*
** Parse the parameter and fill the elfshpath_t
** Change ELFSH_FIELD_SEP in elfsh/elfsh/include/elfsh.h
*/
int			vm_lookup_param(char *param, elfshpath_t *pobj, u_int m)
{
  int			ret;
  char			obj[ELFSH_MEANING];
  char			field1[ELFSH_MEANING];
  char			field2[ELFSH_MEANING];
  char			index[ELFSH_MEANING];
  char			index2[ELFSH_MEANING];
  char			off[ELFSH_MEANING];
  char			sizelem[ELFSH_MEANING];

#if __DEBUG_MODEL__
  printf("[debug_model] Looking up Parameter %s \n", param);
#endif

  /* initialize pobj, it won't hurt */
  bzero(pobj, sizeof(elfshpath_t));

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
      ret = lookup5_index(pobj, obj, field1, index, index2, field2);
      if (!ret)
	return (0);
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
      ret = lookup4(pobj, obj, field1, index, field2, atoi(off), atoi(sizelem));
      if (!ret)
	return (0);
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
      ret = lookup4(pobj, obj, field1, index, field2, atoi(off), 1);
      if (!ret)
	return (0);
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
      ret = lookup4(pobj, obj, field1, index, field2, 0, 1);
      if (!ret)
	return (0);
    }

  /* RULE3: Only 1 level with 1 index : 3 fields [got/ctors/dtors] */
  ret = sscanf(param, 
	       "%41[^"ELFSH_FIELD_SEP"]"
	       ELFSH_FIELD_SEP
	       "%41[^[][%41[^]]]", 
	       obj, field1, index);
  if (ret == 3)
    {
      ret = lookup3_index(pobj, obj, field1, index);
      if (!ret)
	return (0);
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
      ret = lookup3(pobj, obj, field1, field2);
      if (!ret)
	return (0);
    }

  /* RULE5: Try immediate value */
  ret = lookup_immed(param, pobj);
  if (!ret)
    return (0);

  /* No good syntax available, print error if we a not in probe mode */
  if (m)
    vm_badparam(param);
  pobj->type = ELFSH_OBJUNK;
  return (-1);
}





