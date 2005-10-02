/*      
** variables.c for elfsh
**
** Contain functions for easy variables accesses
**
** Started September 16 2005 04:01:03 mayhem
**
*/
#include "elfsh.h"



/* Set a variable to a string value */
int		vm_setvar_str(char *varname, char *value)
{
  elfshpath_t	*var;
  int		len;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (varname != NULL && *varname == ELFSH_VARPREF)
    varname = vm_lookup_var(++varname);

  var = hash_get(&vars_hash, varname);
  if (!var)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));
  
  if (vm_convert2str(var) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed string conversion", (-1));

  len = strlen(value) + 1;
  if (var->size < len)
    XREALLOC(var->immed_val.str, var->immed_val.str, len, -1);
  memcpy(var->immed_val.str, value, len - 1);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}

/* Set a variable to a string value */
int             vm_setvar_raw(char *varname, char *value, u_int len)
{
  elfshpath_t   *var;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (varname != NULL && *varname == ELFSH_VARPREF)
    varname = vm_lookup_var(++varname);
  
  var = hash_get(&vars_hash, varname);
  if (!var)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unknown variable", (-1));
  
  if (vm_convert2raw(var) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed string conversion", (-1));
  
  if (var->size < len)
    XREALLOC(var->immed_val.str, var->immed_val.str, len, -1);
  memcpy(var->immed_val.str, value, len - 1);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}



/* Set a variable to a string value */
int		vm_setvar_byte(char *varname, u_char byte)
{
  elfshpath_t	*var;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (varname != NULL && *varname == ELFSH_VARPREF)
    varname = vm_lookup_var(++varname);

  var = hash_get(&vars_hash, varname);
  if (!var)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));
  
  if (vm_convert2byte(var) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed byte conversion", (-1));

  var->immed_val.byte = byte;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/* Set a variable to a string value */
int		vm_setvar_short(char *varname, u_short val)
{
  elfshpath_t	*var;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (varname != NULL && *varname == ELFSH_VARPREF)
    varname = vm_lookup_var(++varname);

  var = hash_get(&vars_hash, varname);
  if (!var)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));
  
  if (vm_convert2short(var) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed short conversion", (-1));

  var->immed_val.half = val;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/* Set a variable to a string value */
int		vm_setvar_int(char *varname, u_int val)
{
  elfshpath_t	*var;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (varname != NULL && *varname == ELFSH_VARPREF)
    varname = vm_lookup_var(++varname);

  var = hash_get(&vars_hash, varname);
  if (!var)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));
  
  if (vm_convert2int(var) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed integer conversion", (-1));

  var->immed_val.word = val;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/* Set a variable to a string value */
int             vm_setvar_long(char *varname, u_long val)
{
  elfshpath_t   *var;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (varname != NULL && *varname == ELFSH_VARPREF)
    varname = vm_lookup_var(++varname);
  
  var = hash_get(&vars_hash, varname);
  if (!var)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unknown variable", (-1));
  
  if (vm_convert2long(var) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed long conversion", (-1));

  var->immed_val.ent = val;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}
