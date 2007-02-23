/*
**
** lookup.c : Various path lookup functions
**
** Started      21 Nov 2003 mayhem
** Last update  21 Nov 2003 mayhem
**
*/
#include "revm.h"


/* Support for double variables : $$name */
char			*vm_lookup_var(char *param)
{
  revmobj_t		*ptr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);
  
  if (*param == ELFSH_VARPREF)
    {
      ptr = (void *) hash_get(&vars_hash, ++param);
      if (ptr == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown variable", (NULL));
      if (vm_convert_object(ptr, ASPECT_TYPE_STR) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Failed parameter string conversion", 
			  (NULL));
      param = ptr->immed_val.str;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, param);
}



/* Get address value */
revmobj_t		*vm_lookup_addr(char *param)
{
  elfsh_Sym		*sym;
  revmconst_t		*actual;
  char			eol;
  int			ret;
  revmobj_t		*ptr;
  elfsh_Addr	       	val;

#if __DEBUG_LANG__
  char			logbuf[BUFSIZ];

 snprintf(logbuf, BUFSIZ - 1, 
	  "[DEBUG_MODEL] Lookup immed : PARAM [ %s ] \n", param);
 vm_output(logbuf);
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Lookup .symtab */
  sym = elfsh_get_symbol_by_name(world.curjob->current, param);
  if (sym != NULL && sym->st_value > 0)
    {
      ptr = vm_create_LONG(0, sym->st_value);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
    }

  /* Lookup .dynsym */
  sym = elfsh_get_dynsymbol_by_name(world.curjob->current, param);
  if (sym != NULL && sym->st_value > 0)
    {
      ptr = vm_create_LONG(0, sym->st_value);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
    }

  /* Lookup a constant */
  /* FIXME : Constants must be differentiated by their size ! */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    {
      ptr = vm_create_IMMED(ASPECT_TYPE_INT, 0, actual->val);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
    }
  
  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    {
      ptr = vm_create_LONG(0, val);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
    }

  /* No match -- returns ERR */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup address object", (NULL));
}



/* Get immediate value */
revmobj_t		*vm_lookup_immed(char *param)
{
  elfsh_Sym		*sym;
  revmconst_t		*actual;
  char			eol;
  int			ret;
  char			lbuf[4096];
  revmobj_t		*ptr;
  elfsh_Addr	       	val;

#if __DEBUG_LANG__
  char			logbuf[BUFSIZ];

 snprintf(logbuf, BUFSIZ - 1, 
	  "[DEBUG_MODEL] Lookup immed : PARAM [ %s ] \n", param);
 vm_output(logbuf);
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Support for lazy creation of variables */
  if (param != NULL && *param == ELFSH_VARPREF)
    {
      param = vm_lookup_var(++param);
      if (!param)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to lookup variable", (NULL));
      ptr = (void *) hash_get(&vars_hash, param);
      if (ptr != NULL)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
      ptr = vm_create_IMMED(ASPECT_TYPE_UNKNOW, 1, 0);
      hash_add(&vars_hash, aproxy_strdup(param), ptr);
 
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
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
      ptr = vm_create_IMMED(ASPECT_TYPE_INT, 0, actual->val);
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
      ptr = vm_create_IMMEDSTR(0, aproxy_strdup(lbuf));
 
      goto good;
    }

  /* No match -- returns ERR */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup object", (NULL));
  
  /* Match */
 good:
  
  /* Now replace \x00 patterns if any */
  if (ptr->type == ASPECT_TYPE_STR)
    vm_filter_zero(ptr->immed_val.str);

  /* We matched -- returns OK */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
}



/* Lookup an index */
elfsh_Addr     		vm_lookup_index(char *param)
{
  revmconst_t		*actual;
  revmobj_t		*ptr;
  char			eol;
  int			ret;
  elfsh_Addr	       	val;

#if __DEBUG_LANG__
  char			logbuf[BUFSIZ];
  
  snprintf(logbuf, BUFSIZ - 1, "[DEBUG_MODEL] Lookup index : PARAM [ %s ] \n", 
	   param);
  vm_output(logbuf);
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Support for lazy creation of variables */
  if (param != NULL && *param == ELFSH_VARPREF)
    {
      param = vm_lookup_var(++param);
      if (!param)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to lookup variable", (-1));
      ptr = (void *) hash_get(&vars_hash, param);
      if (ptr != NULL && 
	  (ptr->type == ASPECT_TYPE_INT   || 
	   ptr->type == ASPECT_TYPE_SHORT || 
	   ptr->type == ASPECT_TYPE_BYTE  || 
	   ptr->type == ASPECT_TYPE_CADDR ||
	   ptr->type == ASPECT_TYPE_DADDR))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			   ptr->immed_val.ent);
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid variable type", (-1));
    }

  /* Lookup a constant */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       actual->val);
  
  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (val));

  /* Lookup decimal numeric value */
  ret = sscanf(param, UFMT "%c", &val, &eol);
  if (ret == 1)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (val));

  /* We do not match -- returns ERR */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup valid object", (-1));
}



/* Lookup an index */
char			*vm_lookup_string(char *param)
{
  revmobj_t		*ptr;
  char			eol;
  int			ret;
  char			lbuf[4096];

#if __DEBUG_LANG__
  char			logbuf[BUFSIZ];
  
  snprintf(logbuf, BUFSIZ - 1, 
	   "[DEBUG_MODEL] Lookup string : PARAM [ %s ] \n", param);
  vm_output(logbuf);
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Support for lazy creation of variables */
  if (param != NULL && *param == ELFSH_VARPREF)
    {
      param = vm_lookup_var(++param);
      if (!param)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to lookup variable", (NULL));
      ptr = (void *) hash_get(&vars_hash, param);
      if (ptr != NULL && ptr->type == ASPECT_TYPE_STR)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			   ptr->immed_val.str);
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unexpected object type", (NULL));
    }

  /* Lookup a supplied string */
  bzero(lbuf, sizeof(lbuf));
  ret = sscanf(param, "%[^\n]4095%c", lbuf, &eol);
  if (ret == 1)
    {
      vm_filter_zero(lbuf);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 aproxy_strdup(lbuf));
 
    }

  /* We do not match -- returns ERR */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup string", (NULL));
}




/* Lookup the file pointed by name */
elfshobj_t		*vm_lookup_file(char *param)
{
  u_int			idx;
  revmobj_t		*ptr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Support for variable file lookup */
  idx = 0;
  if (param != NULL && *param == ELFSH_VARPREF)
    {
      param = vm_lookup_var(++param);
      if (!param)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to lookup variable", (NULL));
      ptr = (void *) hash_get(&vars_hash, param);
      if (!ptr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get variable", (NULL));
      if (ptr->type == ASPECT_TYPE_INT)
	idx = ptr->immed_val.ent;
      else if (ptr->type == ASPECT_TYPE_STR)
	param = ptr->immed_val.str;
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unexpected variable type", (NULL));
    }
  else
    idx = atoi(param);
  
  /* Let's ask the hash table for the current working file */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (idx ? vm_getfile(idx) : hash_get(&file_hash, param)));
}
