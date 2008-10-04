/**
 * @file access.c
 * @ingroup librevm_lang
 * @brief Implementation of scripting lookups for meta-language variables.
 *
 * Started Jan 23 2007 23:39:51 jfv
 * $Id: access.c,v 1.32 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "revm.h"


/**
 * @brief Return the index for an array access giving a string
 */
int		revm_arrayindex_get(char *strindex)
{
  revmexpr_t	*expr;
  revmobj_t	*obj;
  int		index;
  char		*str;
  int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First remove the ] in a copy of the string */
  str = strdup(strindex + 1);
  for (idx = 0; str[idx] && str[idx] != ']'; idx++);
  if (str[idx])
    str[idx] = 0x00;

  /* Lookup the value of the index string */
  expr = revm_lookup_var(str);
  if (!expr || !expr->value)
    {
      idx = atoi(str);
      XFREE(__FILE__, __FUNCTION__, __LINE__, str);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, idx);
    }
  obj = expr->value;
  switch (obj->otype->type)
    {
    case ASPECT_TYPE_LONG:
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
      index = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      break;
    case ASPECT_TYPE_INT:
      index = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));      
      break;
    case ASPECT_TYPE_SHORT:
      index = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));      
      break;
    case ASPECT_TYPE_BYTE:
      index = (obj->immed ? obj->immed_val.byte : obj->get_obj(obj->parent));      
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Invalid index syntax", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, index);
}


/** 
 * @brief Return the requested projections in case of an array 
 */
int		revm_arrayoff_get(char *field, unsigned int elmsize, 
				  unsigned int dimnbr, unsigned int *dims)
{
  char		*strindex;
  char		*endindex;
  int		offset;
  int		index;
  int		iter;
  int		liter;
  unsigned int		*dimoff;
  char		*lfield;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  dimoff = alloca(dimnbr * sizeof(int));
  iter   = strlen(field);
  lfield = alloca(iter + 1);
  memcpy(lfield, field, iter);

  /* Compute offset for each dimension in the first pass */
  for (iter = 0; 
       lfield && *lfield && iter < dimnbr; 
       lfield = endindex + 1, iter++)
    {
      strindex = strchr(lfield, '[');
      if (strindex)
	{
	  *strindex = 0x00;
	  endindex = strchr(strindex + 1, ']');
	  if (!endindex)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Invalid array syntax", -1);
	  *endindex = 0x00;
	  index = revm_arrayindex_get(strindex);
	  if (index < 0 || index >= dims[iter] || (strindex + 1) == endindex)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Invalid array index", -1);

	  /* Example : [2][3][2] -> (2 * dim2 * dim3 + 3 * dim3 + 2) * sizeof(int) */
	  dimoff[iter] = index * sizeof(int);
	  for (liter = 0; liter != iter; liter++)
	    dimoff[liter] *= dims[iter];
	}
      else
	break;
    }
  
  /* Make sure we havent gone further the limit of dimensions */
  if (iter != dimnbr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid array dimensions", -1);

  /* Compute the final offset of element inside the array by
     adding offsets of all dimensions */
  for (offset = iter = 0; iter < dimnbr; iter++)
    offset += dimoff[iter];

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, offset);
}




/** 
 * @brief Return offset given field name 
 */
aspectype_t	*revm_fieldoff_get(aspectype_t *parent, char *field, unsigned int *off)
{
  aspectype_t	*child;
  int		index;
  char		fieldname[256];
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Truncate the name to keep only the field identifier */
  memcpy(fieldname, field, sizeof(fieldname) - 1);
  str = strchr(fieldname, '[');
  if (str)
    *str = 0x00;

  /* Find the child offset, augment it in case of array */
  for (child = parent->childs; child; child = child->next)
    if (!strcmp(child->fieldname, fieldname))
      {
	*off = child->off;
	
	/* Get offset inside the array if we are accessing an array */
	index = revm_arrayoff_get(field, child->size, 
				child->dimnbr, child->elemnbr);
	if (index < 0)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Invalid array access", NULL);
	*off += index;
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, child);
      }

  /* Failed match */
  *off = REVM_INVALID_FIELD;
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Cannot find requested field offset", NULL);
}


/** 
 * @brief Recursive function to lookup data from its typed data flow path 
 */
static aspectype_t	*revm_field_get(aspectype_t *type, char *param, 
					void **data, char translateaddr)
					
{
  char			*str;
  char			*next;
  unsigned int		off;
  aspectype_t		*child;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  str = strchr(param, *REVM_SEP);

  /* This is the leaf field */
  if (!str)
    {
      child = revm_fieldoff_get(type, param, &off);
      if (off == REVM_INVALID_FIELD)
	{
	  printf("Cannot find field %s in type %s\n", param, type->name);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Cannot find terminal field", NULL);
	}

      *data = (char *) *data + off;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, child);
    }

  /* This is a non-terminal child field */
  *str = 0x00;
  next = str + 1;

  child = revm_fieldoff_get(type, param, &off);
  if (off == REVM_INVALID_FIELD)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find field", NULL);
  *data = (char *) *data + off;

  /* Dereference pointer (struct.ptr->to.other.field) */
  if (child->isptr && *data)
    {
      *data = (void *) *(unsigned long *) *data;
      if (translateaddr)
	*data = elfsh_get_raw_by_addr(world.curjob->curfile, *data);
    }

  if (!*data)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find dereference NULL field", NULL);

  child = revm_field_get(child, next, data, translateaddr);
  if (!child)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested field", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, child);
}



/** 
 * @brief Create the REVM object that is to be returned 
 */
revmobj_t	*revm_object_create(aspectype_t *type, void *data, char translateaddr)
{
  revmobj_t	*path;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Create the revmobj and fill its handlers */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, path, sizeof(revmobj_t), NULL);
  path->root     = (void *) type;

  /* Lookup again in the file if we are dealing with a pointer */
  if (type->type == ASPECT_TYPE_STR || (type->isptr && *(unsigned long *) data))
    {
      data = (void *) *(eresi_Addr *) data;
      if (translateaddr)
	data = elfsh_get_raw_by_addr(world.curjob->curfile, data);
    }

  /* In case of list or hashes */
  if (type->isptr && (type->type == ASPECT_TYPE_LIST || type->type == ASPECT_TYPE_HASH))
    path->get_obj  = (void *) revm_hash_getobj;
  else
    path->get_obj  = (void *) revm_generic_getobj;

  /* Fill type specific object handlers */
  path->parent   = (void *) data;
  if (type->type == ASPECT_TYPE_STR)
    {
      path->get_name = (void *) revm_generic_getname;
      path->set_name = (void *) revm_generic_setname;
    }
  if (!type)
    {
      path->get_data = (void *) revm_generic_getdata;
      path->set_data = (void *) revm_generic_setdata;
    }

  /* This handler is size dependant */
  if (type)
    switch (type->type)
      {
      case ASPECT_TYPE_BYTE:
	path->set_obj  = (void *) revm_byte_setobj;
	break;
      case ASPECT_TYPE_SHORT:
	path->set_obj  = (void *) revm_short_setobj;
	break;
      case ASPECT_TYPE_INT:
	path->set_obj  = (void *) revm_int_setobj;
	break;
      case ASPECT_TYPE_CADDR:
      case ASPECT_TYPE_DADDR:
      case ASPECT_TYPE_LONG:
	path->set_obj  = (void *) revm_long_setobj;
	break;
      default:
	break;
      }
  path->otype = type;
  path->immed = 0;		/* Value is not immediate */
  path->perm  = 1;		/* Do not free after use  */

  /* Success */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, path);
}


/**
 * @brief Lookup _for real_ the path of a complex typed object 
 */
revmobj_t	*revm_object_lookup_real(aspectype_t *type, 
					 char	     *objname, 
					 char	     *objpath,
					 char	     translateaddr)
{
  revmannot_t	*annot;
  void		*data;
  revmobj_t	*path;
  char		hashname[ERESI_MEANING];
  hash_t	*typehash;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_EXPRS__
  fprintf(stderr, "REVMOBJ_LOOKUP_REAL (%s of type %s and wanted field %s)\n", 
	  objname, type->name, objpath);
#endif

  snprintf(hashname, sizeof(hashname), "type_%s", type->name);
  typehash = hash_get(hash_hash, hashname);
  if (!typehash)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested type map", NULL);
  annot = hash_get(typehash, objname);
  if (!annot)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested data object", NULL);
  data = (void *) annot->addr;
  if (translateaddr)
    data = elfsh_get_raw_by_addr(world.curjob->curfile, data);

  /* Get recursively the leaf type and data pointer */
  /* If the objpath is empty, its a scalar that was requested */
  if (objpath && *objpath)
    {
      type = revm_field_get(type, objpath, (void **) &data, translateaddr);
      if (!type)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to lookup object", NULL);
    }

  /* Create and return the object describing the field */
  path = revm_object_create(type, data, translateaddr);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, path);
}


/** 
 * @brief Lookup the path for a complex typed object (using syntactic sugar) 
 */
revmobj_t	*revm_object_lookup(char *str)
{
  char		filename[ERESI_MEANING];
  char		typename[ERESI_MEANING];
  char		objectname[ERESI_MEANING];
  int		ret;
  elfshobj_t	*obj;
  aspectype_t	*type;
  revmobj_t	*path;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#define LOOKUP3_IDX "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^]]]"

  ret = sscanf(str, LOOKUP3_IDX, filename, typename, objectname);
  if (ret != 3)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to resolve manual type object", NULL);
  str += strlen(filename) + strlen(typename) + strlen(objectname) + 3;
  if (str[0] && str[1])
    str++;

  /* Get parent objects */
  obj = revm_lookup_file(filename);
  if (obj == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object", NULL);
  type = hash_get(&types_hash, typename);
  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested type", NULL);

  path = revm_object_lookup_real(type, objectname, str, 1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, path);
}





/**************** Generic handlers *********************************/



char		*revm_generic_getname(void *type, void *data)
{
  return (data);
}

int		revm_generic_setname(void *type, void *data, void *newdata)
{
  strcpy(data, newdata);
  return (0);
}

eresi_Addr	revm_generic_getobj(void *data)
{
  return (*(eresi_Addr *) data);
}

eresi_Addr	revm_hash_getobj(void *data)
{
  return ((eresi_Addr) data);
}

int		revm_byte_setobj(void *data, eresi_Addr value)
{
  unsigned char	*byte;

  byte  = (unsigned char *) data;
  *byte = (unsigned char) value;
  return (0);
}

int		revm_short_setobj(void *data, eresi_Addr value)
{
  u_short	*half;

  half  = (u_short *) data;
  *half = (u_short) value;
  return (0);
}

int		revm_int_setobj(void *data, eresi_Addr value)
{
  unsigned int	*val;

  val  = (unsigned int *) data;
  *val = (unsigned int) value;
  return (0);
}

int		revm_long_setobj(void *data, eresi_Addr value)
{
  eresi_Addr	*dst;

  dst = (eresi_Addr *) data;
  *dst = value;
  return (0);
}

char		*revm_generic_getdata(void *data, int off, int sizelm)
{
  return ((char *) data + (off * sizelm));
}

int		revm_generic_setdata(void *data, int off, void *newdata, 
				   int size, int sizelm)
{
  memcpy((char *) data + (off * sizelm), newdata, size);
  return (0);
}


/**************** End of generic handlers *********************************/
