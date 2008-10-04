/**
 * @defgroup librevm_lang librevm Language
 */
/**
 * @file variables.c
 * @ingroup librevm_lang
 * @brief Contain functions for easy variables accesses.
 *
 * Started September 16 2005 04:01:03 jfv
 *
 *
 * $Id: variables.c,v 1.9 2007-11-28 07:56:09 may Exp $
 *
 */
#include "revm.h"



/**
 * Set a variable to a string value 
 */
int		revm_setvar_str(char *varname, char *value)
{
  revmexpr_t	*expr;
  revmobj_t	*var;
  int		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid NULL parameter", (-1));
  varname = revm_lookup_string(varname);
  expr = revm_expr_get(varname);
  if (!expr || !expr->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot convert variable", (-1));
  var = expr->value;
  if (revm_convert2str(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed string conversion", (-1));
  len = strlen(value) + 1;
  if (var->size < len)
    XREALLOC(__FILE__, __FUNCTION__, __LINE__,
	     var->immed_val.str, var->immed_val.str, len, -1);
  memcpy(var->immed_val.str, value, len - 1);
  var->immed_val.str[len - 1] = '\0';
  expr->type = var->otype;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/** 
 * Set a variable to a string value 
 */
int             revm_setvar_raw(char *varname, char *value, unsigned int len)
{
  revmexpr_t	*expr;
  revmobj_t	*var;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid NULL parameter", (-1));
  varname = revm_lookup_string(varname);
  expr = revm_expr_get(varname);
  if (!expr || !expr->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Cannot convert variable", (-1));
  var = expr->value;
  if (revm_convert2raw(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed string conversion", (-1));
  if (var->size < len)
    XREALLOC(__FILE__, __FUNCTION__, __LINE__,
	     var->immed_val.str, var->immed_val.str, len, -1);
  memcpy(var->immed_val.str, value, len - 1);
  expr->type = var->otype;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}



/** 
 * Set a variable to a string value 
 */
int		revm_setvar_byte(char *varname, unsigned char byte)
{
  revmexpr_t	*expr;
  revmobj_t	*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", (-1));  
  varname = revm_lookup_string(varname);
  expr = revm_expr_get(varname);
  if (!expr || !expr->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot convert variable", (-1));
  var = expr->value;
  if (revm_convert2byte(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed byte conversion", (-1));
  var->immed_val.byte = byte;
  expr->type = var->otype;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/** 
 * Set a variable to a string value 
 */
int		revm_setvar_short(char *varname, u_short val)
{
  revmexpr_t	*expr;
  revmobj_t	*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", (-1));  
  varname = revm_lookup_string(varname);
  expr = revm_expr_get(varname);
  if (!expr || !expr->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));
  var = expr->value;
  if (revm_convert2short(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed short conversion", (-1));
  var->immed_val.half = val;
  expr->type = var->otype;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/** 
 * Set a variable to a string value 
 */
int		revm_setvar_int(char *varname, unsigned int val)
{
  revmexpr_t	*expr;
  revmobj_t	*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", (-1));  
  varname = revm_lookup_string(varname);
  expr = revm_expr_get(varname);
  if (!expr || !expr->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));
  var = expr->value;
  if (revm_convert2int(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed integer conversion", (-1));
  var->immed_val.word = val;
  expr->type = var->otype;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/** 
 * Set a variable to a string value 
 */
int             revm_setvar_long(char *varname, unsigned long val)
{
  revmexpr_t	*expr;
  revmobj_t     *var;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", (-1));  
  varname = revm_lookup_string(varname);
  expr = revm_expr_get(varname);
  if (!expr || !expr->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));
  var = expr->value;
  if (revm_convert2long(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed long conversion", (-1));
  var->immed_val.ent = val;
  expr->type = var->otype;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}



/* Allocate a new temporary variable name */
char		*revm_tmpvar_create()
{
  static unsigned int	lastid = 1;
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, name, 20, NULL);
  snprintf(name, 20, "%s%u", REVM_TMPVAR_PREFIX, lastid);
  lastid++;

  //printf(" [D] Created new temp variable %s \n", name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, name);
}


/* Return 1 if variable is temporary, 0 if not */
int		revm_variable_istemp(revmexpr_t *e)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!e || strncmp(e->label, REVM_TMPVAR_PREFIX, strlen(REVM_TMPVAR_PREFIX)))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}
