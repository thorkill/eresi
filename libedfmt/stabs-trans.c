/*
** @file stabs-trans.c
** @ingroup libedfmt
**
** Started Fev 02 2007 14:35:03 mxatone
**
**
** $Id: stabs-trans.c,v 1.11 2007-06-02 19:18:11 mxatone Exp $
**
*/

#include "libedfmt.h"

/**
 * Translation from stabs representation into uniform format 
 * @file stabs-trans.c
 */

#define STABS_HNAME_TYPE_ADD "stabs_type_added"

/**
 * Type hash table
 * store added type
 */
hash_t types_added;

/**
 * Optimize stack allocation by setting a global pointer for string things 
 */
char buf[BUFSIZ];

/**
 * Advanced iterating of a structure element 
 * @param type type input
 * @param main_type (0 = resolve way, 1 = main way)
 * @return generated uniform type 
 */
static edfmttype_t     	*edfmt_stabs_transform_type_adv(edfmtstabstype_t *type, unsigned char main_type)
{
  edfmttype_t		*etype = NULL;
  edfmttype_t		*stype = NULL;
  edfmttype_t		*ftype = NULL;
  edfmtstabsattr_t	*attr;
  char			*str = NULL;
  int			addtype = 1;
  unsigned char		wasclink = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  /* Do we already have a type for this entry ? */
  if (type->transtype != NULL)
    {
      if (main_type)
	edfmt_change_type_nfile(type->transtype);

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type->transtype);
    }

  /* Thread by type */
  switch(type->type)
    {
    case STABS_TYPE_UNION:
    case STABS_TYPE_STRUCT:
      if (type->data)
	str = type->data->name;

      /* Resolve a name from a parent link */
      if (!str && type->parent_link && type->parent_link->data)
	str = type->parent_link->data->name;

      /* If we didn't have any name, we generate it */
      if (!str)
	{
	  snprintf(buf, BUFSIZ - 1, "s_(%d)", rand() % 99999);
	  str = buf;
	}

      if (type->type == STABS_TYPE_STRUCT)
	{
	  etype = edfmt_add_type_struct(str,
					(int) type->u.struct_union.size);
	}
      else
	{
	  etype = edfmt_add_type_union(str,
				       (int) type->u.struct_union.size);
	}
      
      /* Add the new type to avoid infinite loop */
      if (etype)
	{
	  type->transtype = etype;
	  HASH_ADDX(&types_added, type->cnum, etype);
	  addtype = 0;
	}

      /* Add structure / union members */
      for (attr = type->u.struct_union.attr; attr != NULL; attr = attr->next)
	{
	  if (attr->type)
	    {
	      stype = edfmt_stabs_transform_type_adv(attr->type, 0);

	      ftype = edfmt_add_type_attr(etype, attr->name, 
					  attr->start, attr->size, stype);
	    }
	}
      break;
    case STABS_TYPE_RANGE:
      /* A range is a basic type */
      etype = edfmt_add_type_basic(type->data->name, (int) type->u.r.size);
      break;
    case STABS_TYPE_ARRAY:
      stype = edfmt_stabs_transform_type_adv(type->u.arr.link, 0);

      str = type->data->name;
      if (str == NULL)
	{
	  /* Generate an array type */
	  snprintf(buf, BUFSIZ - 1, "%s[%ld]", stype->name, type->u.arr.high+1);
	  str = buf;	  
	}

      etype = edfmt_add_type_array(str, 
				   type->u.arr.high,
				   stype);      
      break;
    case STABS_TYPE_VOID:
      etype = edfmt_add_type_void(type->data->name);
      break;
    case STABS_TYPE_CONST:
    case STABS_TYPE_FUNC:
    case STABS_TYPE_LINK:
    case STABS_TYPE_REF:
    case STABS_TYPE_PTR:
    case STABS_TYPE_CLINK:
      if (type->type == STABS_TYPE_CLINK)
	{
	  /* Cross reference not found, that can happen */
	  if (type->u.link == NULL)
	    {
	      etype = edfmt_add_type_unk("unk_cref");
	    }
	  else
	    {
	      wasclink = 1;
	      type->type = STABS_TYPE_LINK;
	    }
	}

      if (etype == NULL)
	{
	  stype = edfmt_stabs_transform_type_adv(type->u.link, 0);

	  if (!stype)
	    break;
	  
	  switch(type->type)
	    {
	    case STABS_TYPE_CONST:
	      snprintf(buf, BUFSIZ - 1, "(const:%s)", stype->name);
	      str = buf;
	      break;
	    case STABS_TYPE_FUNC:
	      snprintf(buf, BUFSIZ - 1, "(func:%s)()", stype->name);
	      stype->size = sizeof(void *);
	      str = buf;
	      break;
	    case STABS_TYPE_PTR:
	      stype->size = sizeof(void *);
	    default:
	      str = type->data->name;
	      if (str == NULL || !str[0])
		{
		  if (wasclink)
		    {
		      snprintf(buf, BUFSIZ - 1, "%s", stype->name);
		      str = buf;
		    }
		  else
		    {
		      snprintf(buf, BUFSIZ - 1, "*%s", stype->name);
		      str = buf;
		    }
		}
	    }
	  
	  if (type->type == STABS_TYPE_LINK)
	    etype = edfmt_add_type_link(str, stype);
	  else
	    etype = edfmt_add_type_ptr(str, stype);
	}
      break;
    case STABS_TYPE_VOLATILE:
    case STABS_TYPE_ENUM:
      /* Not yet */
      break;
    }
  
  if (etype && addtype)
    {
      type->transtype = etype;
      HASH_ADDX(&types_added, type->cnum, etype);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, etype);
}

/**
 * This function iterate from normal way 
 * @param type_list input stabs type list
 * @see edfmt_stabs_transform_type_adv
 */
static int		edfmt_stabs_transform_type(edfmtstabstype_t *type_list)
{
  edfmtstabstype_t	*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!type_list)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  for (type = type_list; type != NULL; type = type->next)
    edfmt_stabs_transform_type_adv(type, 1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Transform a variable 
 * @param var_list input stabs variable list
 */
static int		edfmt_stabs_transform_var(edfmtstabsdata_t *var_list)
{
  edfmttype_t 		*stype;
  edfmtstabsdata_t 	*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!var_list)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  for (var = var_list; var != NULL; var = var->next)
    {
      if (!var->type)
	continue;

      /* For the moment we handle only global variables */
      switch(var->scope)
	{
	case STABS_SCOPE_GVAR:
	  stype = edfmt_stabs_transform_type_adv(var->type, 0);

	  edfmt_add_var_global(stype, var->name, var->u.addr);
	  break;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Transform a function 
 * @param func_list input stabs function list
 */
static int		edfmt_stabs_transform_func(edfmtstabsfunc_t *func_list)
{
  edfmtstabsfunc_t	*func;
  edfmtstabsdata_t	*arg_func;
  edfmtfunc_t		*uni_func;
  unsigned int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!func_list)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  /* Stabs function is a simple linked list without any child */
  for (func = func_list; func != NULL; func = func->next)
    {
      if (!func->data || !func->rettype)
	continue;

      uni_func = edfmt_add_func(func->data->name, func->rettype->transtype, 
				func->s_addr, func->e_addr);

      if (!uni_func)
	continue;
      
      /* We add each argument using uni_func pointer */
      for (index = 0; index < STABS_MAX_ARGUMENTS && func->args[index]; index++)
	{
	  arg_func = func->args[index];

	  if (!arg_func->type)
	    continue;

	  switch(arg_func->scope)
	    {
	    case STABS_SCOPE_ARG:
	      edfmt_add_arg(uni_func, arg_func->name, 0, arg_func->u.stackpos, 
			    arg_func->type->transtype);
	      break;
	    }
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Organize the whole transformation of the stabs format 
 * @param sfile tranform this file list
 * @param parent parent file
 */
static int		edfmt_stabs_transform_adv(edfmtstabsfile_t *sfile, 
						  edfmtfile_t *parent)
{
  edfmtfile_t		*mparent;
  edfmtstabsfile_t 	*tfile;
  char			name[EDFMT_NAME_SIZE];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!sfile)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  if (types_added.ent == NULL)
    hash_init(&types_added, STABS_HNAME_TYPE_ADD, 50, ASPECT_TYPE_UNKNOW);

  /* Iterate each element of the file list */
  for (tfile = sfile; tfile != NULL; tfile = tfile->next)
    {
      snprintf(name, EDFMT_NAME_SIZE - 1,
	       "%s%s%s", 
	       tfile->path ? tfile->path : "",
	       tfile->path && tfile->file ? \
	         (tfile->path[strlen(tfile->path)-1] == '/' ? "" : "/") : "",
	       tfile->file ? tfile->file : "");

      mparent = edfmt_add_file(parent, name, tfile->s_addr, tfile->e_addr);

      if (tfile->types)
	edfmt_stabs_transform_type(tfile->types);

      if (tfile->vars)
	edfmt_stabs_transform_var(tfile->vars);
      
      if (tfile->inc)
	edfmt_stabs_transform_adv(tfile->inc, mparent);

      if (tfile->func)
	edfmt_stabs_transform_func(tfile->func);
    }

  hash_empty(STABS_HNAME_TYPE_ADD);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Transform stabs informations - Entry point 
 * @param file file to transform from stabs to uniform format
 * @see edfmt_stabs_transform_adv
 */
int			edfmt_stabs_transform(elfshobj_t *file)
{
  edfmtstabsinfo_t 	*tinfo;
  edfmtstabsfile_t 	*tfile;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1); 

  tinfo = (edfmtstabsinfo_t *) file->debug_format.stabs;

  if (tinfo == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No debug information for stabs", -1); 

  tfile = (edfmtstabsfile_t *) tinfo->file;

  if (tfile == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No file information for stabs", -1); 

  /* Init api */
  edfmt_add_init(file);

  edfmt_stabs_transform_adv(tfile, NULL);

  /* Reset api */
  edfmt_add_end();
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
