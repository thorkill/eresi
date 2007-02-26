/*
** edfmt.c for elfsh
**
** All functions dealing with libedfmt
**
** Started on Fev 25 2007 mxatone
*/
#include "revm.h"

char buf[BUFSIZ];

#define TYPE_MAX_ATTR 50
#define TYPE_ATTR_LEN 256
char *tmp_arr[TYPE_MAX_ATTR];
char arr_pool[TYPE_ATTR_LEN];

static int		vm_edfmt_type_parse(edfmttype_t *type)
{
  edfmttype_t		*child;
  int			index = 0;
  char			*subtype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch(type->type)
    {
    case EDFMT_TYPE_UNK:
      break;
    case EDFMT_TYPE_ARRAY:
      if (type->parent)
	{
	  vm_edfmt_type_parse(type->parent);
	  snprintf(arr_pool, TYPE_ATTR_LEN - 1,
		   "elm:%s[%d]", type->parent->name, type->size+1);
	  tmp_arr[index] = strdup(arr_pool);
	  snprintf(arr_pool, TYPE_ATTR_LEN - 1,
		   "array%d%s", type->size+1, type->parent->name);
	  
	  aspect_type_register(strdup(arr_pool), tmp_arr, 1);      
	}
      break;
    case EDFMT_TYPE_PTR:
      if (type->parent)
	vm_edfmt_type_parse(type->parent);
      /*
	{
	  vm_edfmt_type_parse(type->parent);
	  snprintf(arr_pool, TYPE_ATTR_LEN - 1,
		   "elm:%s", type->parent->name);
	  tmp_arr[index] = strdup(arr_pool);

	  aspect_type_register(type->name, tmp_arr, 1);
	}
      */
      break;
    case EDFMT_TYPE_VOID:
    case EDFMT_TYPE_BASIC:
      aspect_basetype_register(type->name, type->size);
      break;
    case EDFMT_TYPE_UNION:
      snprintf(arr_pool, TYPE_ATTR_LEN - 1,
	       "elm%%%d", type->size);
      tmp_arr[index] = strdup(arr_pool);
      aspect_type_register(type->name, tmp_arr, 1);
      break;
    case EDFMT_TYPE_STRUCT:
      for (child = type->child, index = 0; child != NULL; child = child->next, index++)
	{
	  if (child->parent == NULL || child->child == NULL)
	    continue;
		
	  /* We try to avoid infinite loops */
	  if (child->child != type && child->child->type != EDFMT_TYPE_PTR)
	    vm_edfmt_type_parse(child->child);

	  snprintf(arr_pool, TYPE_ATTR_LEN - 1,
		   "%s:%s", child->name, child->child->name);
	  tmp_arr[index] = strdup(arr_pool);
	}
      aspect_type_register(type->name, tmp_arr, index);
      break;
    case EDFMT_TYPE_LINK:
      if (type->parent)
	vm_type_copy(type->parent->name, type->name);
      break;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Add global type into ERESI engine */
static int		vm_edfmt_types(edfmttype_t *types)
{
  edfmttype_t		*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (type = types; type != NULL; type = type->next)
    vm_edfmt_type_parse(type);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Add global variable into ERESI engine */
static int		vm_edfmt_inform(edfmtvar_t *vars)
{
  edfmtvar_t		*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (var = vars; var != NULL; var = var->next)
    {
      snprintf(buf, BUFSIZ - 1, XFMT, vars->addr);
      vm_inform_type(var->type->name, var->name, buf, 0);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Iterate each file */
static int	        vm_edfmt_file(edfmtfile_t *files)
{
  edfmtfile_t		*file;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (file = files; file != NULL; file = file->next)
    {
      vm_edfmt_types(file->types);
      vm_edfmt_inform(file->vars);

      if (file->child)
	vm_edfmt_file(file->child);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Manage libedfmt and fill inform / types from debuggin informations */
int			vm_edfmt_parse(elfshobj_t *file)
{
  edfmtinfo_t		*info;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Parse all debugging information */
  edfmt_format(file);

  /* Retrieve uni format ptr */
  info = edfmt_get_uniinfo(file);

  /* This pointer should be NULL only if there's not "parsable" deubgging
     informations */
  if (info)
    {
      /* Global elements */
      vm_edfmt_types(info->types);
      vm_edfmt_inform(info->vars);

      /* Then local file elements */
      vm_edfmt_file(info->files);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
