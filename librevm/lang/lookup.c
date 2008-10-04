/**
 * @file lookup.c
 * @ingroup librevm_lang
 * @brief Various object lookup functions built in the language.
 *
 * Started Nov 21 2003 jfv
 * $Id: lookup.c,v 1.20 2008-02-16 12:32:27 thor Exp $
 */
#include "revm.h"


/** 
 * @brief Support for double (or multiple) variables : $$name, $$$name, etc 
 */
revmexpr_t		*revm_lookup_var(char *param)
{
  revmexpr_t		*e;
  revmobj_t		*ptr;
  int			indir;
  char			namebuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);

  for (indir = 0; *param == REVM_VAR_PREFIX; indir++, param++);  
  e = NULL;
  ptr = NULL;
  if (!*param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);
  while (indir--)
    {
      snprintf(namebuf, sizeof(namebuf), "$%s", param);
      e = revm_expr_get(namebuf);
      if (!e)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown variable", NULL);
      ptr = e->value;
      if (indir)
	{
	  if (!ptr)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Malformed variable", (NULL));
	  if (revm_convert_object(e, ASPECT_TYPE_STR) < 0 || !ptr->immed)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Cannot convert variable to string", (NULL));
	  param = ptr->immed_val.str;
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, e);
}


/** 
 * @brief Get address value 
 */
eresi_Addr		revm_lookup_addr(char *param)
{
  elfsh_Sym		*sym;
  revmconst_t		*actual;
  char			eol;
  int			ret;
  revmobj_t		*ptr;
  revmexpr_t		*expr;
  eresi_Addr	       	val;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", 0);

  /* Lookup .symtab */
  sym = elfsh_get_symbol_by_name(world.curjob->curfile, param);
  if (sym != NULL && sym->st_value > 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sym->st_value);

  /* Lookup .dynsym */
  sym = elfsh_get_dynsymbol_by_name(world.curjob->curfile, param);
  if (sym != NULL && sym->st_value > 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sym->st_value);

  /* Lookup a variable */
  expr = revm_lookup_var(param);
  if (expr && expr->value)
    {
      ptr = expr->value;
      if (ptr->otype->type == ASPECT_TYPE_LONG  ||
	  ptr->otype->type == ASPECT_TYPE_CADDR ||
	  ptr->otype->type == ASPECT_TYPE_DADDR)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		      (ptr->immed ? ptr->immed_val.ent : 
		       (eresi_Addr) ptr->get_obj(ptr->parent)));
    }

  /* Lookup a constant */
  /* FIXME : Constants must be differentiated by their size ! */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, actual->val);
  
  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, val);

  /* No match -- returns ERR */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup address object", 0);
}



/** 
 * @brief Get immediate value 
 */
revmobj_t		*revm_lookup_immed(char *param)
{
  elfsh_Sym		*sym;
  revmconst_t		*actual;
  char			eol;
  int			ret;
  char			lbuf[4096];
  revmobj_t		*ptr;
  revmexpr_t		*expr;
  eresi_Addr	       	val;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (NULL));

  /* Lookup a known variable */
  expr = revm_lookup_var(param);
  if (expr && expr->value)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr->value);

  /* Support for lazy creation of variables */
  if (*param == REVM_VAR_PREFIX)
    {
      ptr = revm_create_IMMED(ASPECT_TYPE_UNKNOW, 1, 0);
      expr = revm_expr_create_from_object(ptr, param);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
    }

  /* Not necessary to call those functions if there is no loaded file */
  if (world.curjob->curfile)
    {

      /* Lookup .symtab */
      sym = elfsh_get_symbol_by_name(world.curjob->curfile, param);
      if (sym != NULL && sym->st_value)
	{
	  ptr = revm_create_LONG(0, sym->st_value);
	  goto good;
	}
      
      /* Lookup .dynsym */
      sym = elfsh_get_dynsymbol_by_name(world.curjob->curfile, param);
      if (sym != NULL && sym->st_value)
	{
	  ptr = revm_create_LONG(0, sym->st_value);
	  goto good;
	}
    }


  /* FIXME: Constants must be differentiated by their size ! */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    {
      ptr = revm_create_IMMED(ASPECT_TYPE_INT, 0, actual->val);
      goto good;
    }

  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    {
      ptr = revm_create_LONG(0, val);
      goto good;
    }

  /* Lookup decimal numeric value */
  ret = sscanf(param, UFMT "%c", &val, &eol);
  if (ret == 1)
    {
      ptr = revm_create_LONG(0, val);
      goto good;
    }
  
  /* Lookup a supplied string */
  bzero(lbuf, sizeof(lbuf));
  ret = sscanf(param, "%[^\n]4095%c", lbuf, &eol);
  if (ret == 1)
    {
      ptr = revm_create_IMMEDSTR(0, strdup(lbuf));
 
      goto good;
    }

  /* No match -- returns ERR */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup object", (NULL));
  
  /* Match */
 good:
  
  /* Now replace \x00 patterns if any */
  if (ptr->otype->type == ASPECT_TYPE_STR)
    revm_filter_zero(ptr->immed_val.str);

  /* We matched -- returns OK */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
}



/** 
 * @brief Lookup an index 
 */
unsigned int     		revm_lookup_index(char *param)
{
  revmconst_t		*actual;
  revmobj_t		*ptr;
  revmexpr_t		*expr;
  char			eol;
  int			ret;
  eresi_Addr	       	val;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", 0);

  /* Support for lazy creation of variables */
  expr = revm_lookup_var(param);
  if (expr && expr->value)
    {
      ptr = expr->value;
      if (ptr->otype->type == ASPECT_TYPE_INT   || 
	  ptr->otype->type == ASPECT_TYPE_SHORT || 
	  ptr->otype->type == ASPECT_TYPE_BYTE  || 
	  ptr->otype->type == ASPECT_TYPE_LONG  ||
	  ptr->otype->type == ASPECT_TYPE_CADDR ||
	  ptr->otype->type == ASPECT_TYPE_DADDR)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		      (ptr->immed ? (unsigned int) ptr->immed_val.ent : 
		       (unsigned int) ptr->get_obj(ptr->parent)));
      
      if (revm_convert_object(expr, ASPECT_TYPE_INT) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid parameter", 0);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    (ptr->immed ? (unsigned int) ptr->immed_val.ent : 
		     (unsigned int) ptr->get_obj(ptr->parent)));
    }

  /* Lookup a constant */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (unsigned int) actual->val);
  
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
		    "Unable to lookup valid object", ((unsigned int) -1));
}



/** 
 * @brief Lookup an index 
 */
char			*revm_lookup_string(char *param)
{
  revmexpr_t		*expr;
  revmobj_t		*ptr;
  char			*oldparam;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid NULL parameter", (NULL));

  oldparam = param;
  param = strdup(param);

  /* Support for lazy creation of variables */
  expr = revm_lookup_var(param);
  if (!expr || !expr->value)
    {
      revm_filter_zero(param);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, param);
    }
  ptr = expr->value;
  if (ptr->otype->type != ASPECT_TYPE_STR)
    {
      revm_convert_object(expr, ASPECT_TYPE_STR);
      if (ptr->otype->type != ASPECT_TYPE_STR)
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__, param);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unexpected object type", NULL);
	}
    }
  if (ptr->get_name)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		  ptr->get_name(ptr->root, ptr->parent));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		(ptr->immed ? ptr->immed_val.str : 
		 (char *) ptr->get_obj(ptr->parent)));
}



/** 
 * @brief Lookup the file pointed by name 
 */
elfshobj_t		*revm_lookup_file(char *param)
{
  unsigned int			idx;
  revmobj_t		*ptr;
  revmexpr_t		*expr;
  elfshobj_t		*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Support for variable file lookup */
  idx = 0;
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid NULL parameter", (NULL));
  expr = revm_lookup_var(param);
  if (!expr || !expr->value)
    idx = atoi(param);
  else
    {
      ptr = expr->value;
      if (ptr->otype->type == ASPECT_TYPE_INT)
	idx = ptr->immed_val.ent;
      else if (ptr->otype->type == ASPECT_TYPE_STR)
	param = ptr->immed_val.str;
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unexpected variable type", (NULL));
    }

  /* Let's ask the hash table for the current working file */
  ret = (idx ? revm_getfile(idx) : hash_get(&file_hash, param));
  if (ret == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "File not found", (NULL));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}





/** 
 * Parse the parameter and fill the revmobj_t 
 */
revmexpr_t		*revm_lookup_param(char *param)
{
  revmobj_t		*(*funcptr)(char *param, char *fmt, unsigned int sepnbr);
  char			**keys;
  int			keynbr;
  char			*parm;
  unsigned int		sepnbr;
  unsigned int		index;
  revmobj_t		*res;
  revmexpr_t		*expr;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  expr = revm_expr_get(param);
  if (expr)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);  

  /* Find the number of fields in param */
  for (sepnbr = 0, parm = param; *parm; parm++)
    if (*parm == REVM_SEP[0] || *parm == ':' || *parm == '%')
      {
	if (parm > param && parm[-1] == '[')
	  continue;
	sepnbr++;
      }
  
  /* Find the correct parsing handler */
  keys = hash_get_keys(&parser_hash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      funcptr = hash_get(&parser_hash, keys[index]);
      res = funcptr(param, keys[index], sepnbr);
      if (!res)
	continue;
      expr = revm_expr_create_from_object(res, param);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);  
    }

  /* If still not found, try manually inserted types */
  res = revm_object_lookup(param);
  if (res)
    {
      expr = revm_expr_create_from_object(res, param);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);  
    }

  /* If no good syntax is available, print error if we are not in probe mode */
  res = revm_lookup_immed(param);
  if (!res)
    {
      revm_badparam(param);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to resolve object", NULL);
    }

  expr = revm_expr_create_from_object(res, param);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}
