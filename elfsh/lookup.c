/*
**
** lookup.c : Various path lookup functions
**
** Started      21 Nov 2003 mayhem
** Last update  21 Nov 2003 mayhem
**
*/
#include "elfsh.h"


/* Support for double variables : $$name */
char			*vm_lookup_var(char *param)
{
  elfshpath_t		*ptr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);
  
  if (*param == ELFSH_VARPREF)
    {
      ptr = (void *) hash_get(&vars_hash, ++param);
      if (ptr == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown variable", (NULL));
      if (vm_convert_object(ptr, ELFSH_OBJSTR) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Failed parameter string conversion", 
			  (NULL));
      param = ptr->immed_val.str;
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, param);
}


/* Get immediate value */
elfshpath_t		*vm_lookup_immed(char *param)
{
  elfsh_Sym		*sym;
  elfshconst_t		*actual;
  char			eol;
  int			ret;
  char			lbuf[4096];
  elfshpath_t		*ptr;
  elfsh_Addr	       	val;

#if __DEBUG_MODEL__
  char			logbuf[BUFSIZ];

 snprintf(logbuf, BUFSIZ - 1, 
	  "[DEBUG_MODEL] Lookup immed : PARAM [ %s ] \n", param);
 vm_output(logbuf);
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Support for lazy creation of variables */
  if (param != NULL && *param == ELFSH_VARPREF)
    {
      param = vm_lookup_var(++param);
      if (!param)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to lookup variable", (NULL));
      ptr = (void *) hash_get(&vars_hash, param);
      if (ptr != NULL)
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
      ptr = vm_create_IMMED(ELFSH_OBJUNK, 1, 0);
      hash_add(&vars_hash, elfsh_strdup(param), ptr);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
    }

  /* Lookup .symtab */
  sym = elfsh_get_symbol_by_name(world.curjob->current, param);
  if (sym != NULL)
    {
      //printf("symbol_by_name returned %016llX for %s \n", sym->st_value, param);
      ptr = vm_create_LONG(0, sym->st_value);
      goto good;
    }

  /* Lookup .dynsym */
  sym = elfsh_get_dynsymbol_by_name(world.curjob->current, param);
  if (sym != NULL)
    {
      //printf("dynsymbol_by_name returned %016llX for %s \n", sym->st_value, param);
      ptr = vm_create_LONG(0, sym->st_value);
      goto good;
    }

  /* Lookup a constant XXXXX: Constants must be differentiated by their size ! (INT or LONG ?) */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    {
      ptr = vm_create_IMMED(ELFSH_OBJINT, 0, actual->val);
      goto good;
    }

  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    {
      ptr = vm_create_LONG(0, val);
      goto good;
    }

  /* Lookup decimal numeric value */
  ret = sscanf(param, UFMT "%c", &val, &eol);
  if (ret == 1)
    {
      ptr = vm_create_LONG(0, val);
      goto good;
    }

  /* Lookup a supplied string */
  bzero(lbuf, sizeof(lbuf));
  ret = sscanf(param, "%[^\n]4095%c", lbuf, &eol);
  if (ret == 1)
    {
      ptr = vm_create_IMMEDSTR(0, elfsh_strdup(lbuf));
      goto good;
    }

  /* No match -- returns ERR */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup object", (NULL));
  
  /* Match */
 good:
  
  /* Now replace \x00 patterns if any */
  if (ptr->type == ELFSH_OBJSTR)
    vm_filter_zero(ptr->immed_val.str);

  /* We matched -- returns OK */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
}



/* Lookup an index */
elfsh_Addr     		vm_lookup_index(char *param)
{
  elfshconst_t		*actual;
  elfshpath_t		*ptr;
  char			eol;
  int			ret;
  elfsh_Addr	       	val;

#if __DEBUG_MODEL__
  char			logbuf[BUFSIZ];
  
  snprintf(logbuf, BUFSIZ - 1, "[DEBUG_MODEL] Lookup index : PARAM [ %s ] \n", param);
  vm_output(logbuf);
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Support for lazy creation of variables */
  if (param != NULL && *param == ELFSH_VARPREF)
    {
      param = vm_lookup_var(++param);
      if (!param)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to lookup variable", (-1));
      ptr = (void *) hash_get(&vars_hash, param);
      if (ptr != NULL && 
	  (ptr->type == ELFSH_OBJINT   || 
	   ptr->type == ELFSH_OBJSHORT || 
	   ptr->type == ELFSH_OBJBYTE  || 
	   ptr->type == ELFSH_OBJLONG 
	   ))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			   ptr->immed_val.ent);
      else
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid variable type", (-1));
    }

  /* Lookup a constant */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       actual->val);
  
  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (val));

  /* Lookup decimal numeric value */
  ret = sscanf(param, UFMT "%c", &val, &eol);
  if (ret == 1)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (val));

  /* We do not match -- returns ERR */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup valid object", (-1));
}



/* Lookup an index */
char			*vm_lookup_string(char *param)
{
  elfshpath_t		*ptr;
  char			eol;
  int			ret;
  char			lbuf[4096];

#if __DEBUG_MODEL__
  char			logbuf[BUFSIZ];
  
  snprintf(logbuf, BUFSIZ - 1, 
	   "[DEBUG_MODEL] Lookup string : PARAM [ %s ] \n", param);
  vm_output(logbuf);
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Support for lazy creation of variables */
  if (param != NULL && *param == ELFSH_VARPREF)
    {
      param = vm_lookup_var(++param);
      if (!param)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to lookup variable", (NULL));
      ptr = (void *) hash_get(&vars_hash, param);
      if (ptr != NULL && ptr->type == ELFSH_OBJSTR)
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			   ptr->immed_val.str);
      else
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unexpected object type", (NULL));
    }

  /* Lookup a supplied string */
  bzero(lbuf, sizeof(lbuf));
  ret = sscanf(param, "%[^\n]4095%c", lbuf, &eol);
  if (ret == 1)
    {
      vm_filter_zero(lbuf);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 elfsh_strdup(lbuf));
    }

  /* We do not match -- returns ERR */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup string", (NULL));
}




/* Lookup the file pointed by name */
elfshobj_t		*vm_lookup_file(char *param)
{
  u_int			idx;
  elfshpath_t		*ptr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Support for variable file lookup */
  idx = 0;
  if (param != NULL && *param == ELFSH_VARPREF)
    {
      param = vm_lookup_var(++param);
      if (!param)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to lookup variable", (NULL));
      ptr = (void *) hash_get(&vars_hash, param);
      if (!ptr)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get variable", (NULL));
      if (ptr->type == ELFSH_OBJINT)
	idx = ptr->immed_val.ent;
      else if (ptr->type == ELFSH_OBJSTR)
	param = ptr->immed_val.str;
      else
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unexpected variable type", (NULL));
    }
  else
    idx = atoi(param);
  
  /* Let's ask the hash table for the current working file */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (idx ? vm_getfile(idx) : hash_get(&file_hash, param)));
}
