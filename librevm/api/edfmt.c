/*
** @file edfmt.c
** @ingroup librevm_api
** @brief All functions dealing with libedfmt.
**
** Started on Fev 25 2007 mxatone
**
** $Id: edfmt.c,v 1.15 2008-02-16 12:32:27 thor Exp $
**
*/
#include "revm.h"

#define VM_EDFMT_VAR_QUEUE_NAME "revm_edfmt_varqueue"
#define TYPE_MAX_ATTR 500
#define TYPE_ATTR_LEN 256

char *tmp_arr[TYPE_MAX_ATTR];
char arr_pool[TYPE_ATTR_LEN];

#define VM_EDFMT_PARSED(_type) \
_type->parsed = 1

hash_t var_queue;

/* Register a type (and transform its name if needed) */
static int		revm_edfmt_register_type(char isunion,
						 char *label, 
						 char **fields, 
						 unsigned int fieldnbr)
{

  char			buf[BUFSIZ];
  int			index = 0, len;
  int 			ret = 0;
  char			*nlabel;
  aspectype_t		*cur, *new, *inew, *icur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  snprintf(buf, BUFSIZ - 1, "%s", label);
  len = strlen(buf);
  for (index = 0; index < len && buf[index] == '*'; index++)
    buf[index] = 'p';
  nlabel = (char *) strdup(label);

  /* We create the type right now to compare to an existing
     type in case of double entry */
  new = aspect_type_create(isunion, nlabel, fields, fieldnbr);
  if (!new)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid type declaration", -1);

  /* Check if this type already exist */
  cur = hash_get(&types_hash, nlabel);
  if (cur)
    {
      /* Same types ? Compare field names & type names */
      for (inew = new->childs, icur = cur->childs; 
	   inew && icur; inew = inew->next, icur = icur->next)
	if (strcmp(inew->fieldname, icur->fieldname) != 0
	    || strcmp(inew->name, icur->name) != 0)
	  goto diff;

      /* Are we okay ? */
      if (icur == inew)
	goto add;

    diff:
      snprintf(buf, BUFSIZ - 1, " [!] %s has 2 different versions \n",
	       nlabel);
      revm_output(buf);
      revm_output(" [!] New type has been discarded \n\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
    }

 add:
  ret = aspect_type_register_real(nlabel, new);
  if (ret == 0)
    revm_type_hashcreate(nlabel);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/* Analyze a given uniform type and add it into ERESI type engine */
static int		revm_edfmt_type_parse(edfmttype_t *type)
{
  edfmttype_t		*child = NULL;
  int			index = 0;
  int			typeid;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!type || type->parsed)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  VM_EDFMT_PARSED(type);

  typeid = type->type;

 start:

  switch(typeid)
    {
    case EDFMT_TYPE_UNK:
      break;
    case EDFMT_TYPE_ARRAY:
      if (type->parent)
	{
	  revm_edfmt_type_parse(type->parent);

	  snprintf(arr_pool, TYPE_ATTR_LEN - 1,
		   "elm:%s[%d]", type->parent->name, type->size+1);
	  tmp_arr[index] = strdup(arr_pool);
	  snprintf(arr_pool, TYPE_ATTR_LEN - 1,
		   "array%d%s", type->size+1, type->parent->name);
	  
	  revm_edfmt_register_type(0, arr_pool, tmp_arr, 1);  
	}
      break;
    case EDFMT_TYPE_PTR:
      if (type->parent)
	{
	  revm_edfmt_type_parse(type->parent);

	  snprintf(arr_pool, TYPE_ATTR_LEN - 1,
		   "elm:%s", type->parent->name);
	  tmp_arr[index] = strdup(arr_pool);
	  revm_edfmt_register_type(0, type->name, tmp_arr, 1);
	}
      break;
    case EDFMT_TYPE_VOID:
    case EDFMT_TYPE_BASIC:
      aspect_basetype_register(type->name, type->size);
      break;
    case EDFMT_TYPE_UNION:
      snprintf(arr_pool, TYPE_ATTR_LEN - 1,
	       "elm%%%d", type->size);
      tmp_arr[index] = strdup(arr_pool);
      revm_edfmt_register_type(1, type->name, tmp_arr, 1);
      break;
    case EDFMT_TYPE_STRUCT:
      /* Resolve all dependencies before */
      for (child = type->child, index = 0; child != NULL; child = child->next, index++)
	{
	  /* We try to avoid infinite loops */
	  if (child->child && child->child != type)
	    revm_edfmt_type_parse(child->child);
	}

      for (child = type->child, index = 0; 
	   child != NULL && index < TYPE_MAX_ATTR; 
	   child = child->next, index++)
	{
	  if (child->parent == NULL || child->child == NULL)
	    continue;

	  VM_EDFMT_PARSED(child);

	  snprintf(arr_pool, TYPE_ATTR_LEN - 1,
		   "%s:%s", child->name, child->child->name);
	  tmp_arr[index] = strdup(arr_pool);
	}

      revm_edfmt_register_type(0, type->name, tmp_arr, index);
      break;
    case EDFMT_TYPE_LINK:
      if (type->parent)
	{
	  revm_edfmt_type_parse(type->parent);
	  if (type->name[0] == '*')
	    {
	      typeid = EDFMT_TYPE_PTR;
	      goto start;
	    }
	  else
	    {
	      if (revm_type_copy(type->parent->name, type->name) == 0)
		revm_type_hashcreate(type->name);
	    }
	}
      break;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Add global type into ERESI engine */
static int		revm_edfmt_types(edfmttype_t *types)
{
  edfmttype_t		*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (type = types; type != NULL; type = type->next)
    if (type->valid)
      revm_edfmt_type_parse(type);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Add global variable into ERESI engine */
static int		revm_edfmt_inform(edfmtvar_t *var)
{
  int			index = 0, len;
  char			*str;
  char			buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!var)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);  

  snprintf(buf, BUFSIZ - 1, "%s", var->name);
  len = strlen(buf);
  
  for (index = 0; index < len && buf[index] == '*'; index++)
    buf[index] = 'p';

  snprintf(buf, BUFSIZ - 1, XFMT, var->addr);
  if (var->type->type == EDFMT_TYPE_ARRAY)
    {
      snprintf(arr_pool, TYPE_ATTR_LEN - 1,
	       "array%d%s", var->type->size+1, var->type->parent->name);
      str = arr_pool;
    }
  else
    str = var->type->name;

  revm_inform_type(str, buf, buf, NULL, 0, 0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Add some global variable waiting to be added at the end */
static int		revm_edfmt_add_var_queue(edfmtvar_t *vars)
{
  edfmtvar_t		*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!vars)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);  

  if (var_queue.ent == NULL)
    hash_init(&var_queue, VM_EDFMT_VAR_QUEUE_NAME, 50, ASPECT_TYPE_UNKNOW);

  for (var = vars; var != NULL; var = var->next)
    {
      /* We add the variable only if we don't already have it ! */
      if (hash_get(&var_queue, var->name) == NULL)
	hash_add(&var_queue, strdup(var->name), (void *) var);
    }
      
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}

/* Proceed variable waiting to be added, then clean the hash table */
static int		revm_edfmt_proceed_var_queue()
{
  char			**keys;
  unsigned int			index;
  int			keynbr = 0;
  edfmtvar_t		*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  keys = hash_get_keys(&var_queue, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  var = hash_get(&var_queue, keys[index]);

	  if (var)
	    revm_edfmt_inform(var);
	}

      hash_free_keys(keys);

      hash_empty(VM_EDFMT_VAR_QUEUE_NAME);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);    
}

/* Iterate each file */
static int	        revm_edfmt_file(edfmtfile_t *files)
{
  edfmtfile_t		*file;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (file = files; file != NULL; file = file->next)
    {
      revm_edfmt_types(file->types);
      revm_edfmt_add_var_queue(file->vars);

      if (file->child)
	revm_edfmt_file(file->child);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Manage libedfmt and fill inform / types from debuggin informations */
int			revm_edfmt_parse(elfshobj_t *file)
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
      revm_edfmt_types(info->types);

      revm_edfmt_add_var_queue(info->vars);

      /* Then local file elements */
      revm_edfmt_file(info->files);

      /* Add register variables */
      revm_edfmt_proceed_var_queue();
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
