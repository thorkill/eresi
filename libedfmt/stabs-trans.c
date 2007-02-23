/*
** stabs-trans.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Fev 02 2007 14:35:03 mxatone
**
*/

#include "libedfmt.h"

/* Translation from stabs representation into uniform format */

#define STABS_HNAME_TYPE_ADD "stabs_type_added"

hash_t types_added;

static edfmttype_t     	*edfmt_stabs_transform_type_get(edfmtstabstype_t *type)
{
  edfmttype_t		*stype = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  stype = (edfmttype_t *) hash_get(&types_added, type->cnum);

  if (stype)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, stype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

static edfmttype_t     	*edfmt_stabs_transform_type_adv(edfmtstabstype_t *type, u_char main_type)
{
  edfmttype_t		*etype = NULL;
  edfmttype_t		*stype = NULL;
  edfmttype_t		*ftype = NULL;
  edfmtstabsattr_t	*attr;
  char			buf[BUFSIZ];
  char			*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  if (type->transtype != NULL)
    {
      if (main_type)
	edfmt_change_type_nfile(type->transtype);

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type->transtype);
    }

  switch(type->type)
    {
    case STABS_TYPE_UNION:
    case STABS_TYPE_STRUCT:
      if (type->type == STABS_TYPE_STRUCT)
	{
	  etype = edfmt_add_type_struct(type->data->name,
					(int) type->u.struct_union.size);
	}
      else
	{
	  etype = edfmt_add_type_union(type->data->name,
				       (int) type->u.struct_union.size);
	}

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
      etype = edfmt_add_type_basic(type->data->name, 0);
      break;
    case STABS_TYPE_ARRAY:
      stype = edfmt_stabs_transform_type_adv(type->u.arr.link, 0);

      str = type->data->name;
      if (str == NULL)
	{
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
    case STABS_TYPE_LINK:
    case STABS_TYPE_REF:
    case STABS_TYPE_PTR:
      stype = edfmt_stabs_transform_type_adv(type->u.link, 0);

      str = type->data->name;
      if (str == NULL || !str[0])
	{
	  snprintf(buf, BUFSIZ - 1, "*%s", stype->name);
	  str = buf;
	}

      if (type->type == STABS_TYPE_LINK)
	etype = edfmt_add_type_link(str, stype);
      else
	etype = edfmt_add_type_ptr(str, stype);
      break;
    case STABS_TYPE_FUNC:
    case STABS_TYPE_VOLATILE:
    case STABS_TYPE_CONST:
    case STABS_TYPE_ENUM:
      /* Not yet */
      break;
    }
  
  if (etype)
    {
      type->transtype = etype;
      HASH_ADDX(&types_added, type->cnum, etype);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, etype);
}

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

  /* Iterate each element */
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
    }

  hash_empty(STABS_HNAME_TYPE_ADD);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Transform stabs informations */
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
