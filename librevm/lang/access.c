/**
* @file librevm/lang/access.c
 * @ingroup lang
 * @brief Implementation of scripting lookups for meta-language variables.
 *
 * Started Jan 23 2007 23:39:51 jfv
 * $Id$
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
int		revm_arrayoff_get(char *field, u_int elmsize, 
				  u_int dimnbr, u_int *dims)
{
  char		*strindex;
  char		*endindex;
  int		offset;
  int		index;
  u_int		iter;
  int		liter;
  u_int		*dimoff;
  char		*lfield;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  dimoff = alloca(dimnbr * sizeof(int));
  iter   = strlen(field);
  lfield = alloca(iter + 1);
  memcpy(lfield, field, iter);
  lfield[iter] = 0x00;

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
      else if (iter == 0)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Missing array indexes", -1);
    }
  
  /* Make sure we havent gone further the limit of dimensions */
  if (iter != dimnbr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Too many array dimensions", -1);

  /* Compute the final offset of element inside the array by
     adding offsets of all dimensions */
  for (offset = iter = 0; iter < dimnbr; iter++)
    offset += dimoff[iter];

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, offset);
}




/** 
 * @brief Return offset given field name 
 */
aspectype_t	*revm_fieldoff_get(aspectype_t *parent, char *field, u_int *off, u_char *array)
{
  aspectype_t	*child;
  int		index;
  char		fieldname[256];
  int		len;
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Truncate the name to keep only the field identifier */
  len = strlen(field);
  if (len >= sizeof(fieldname) - 1) len = sizeof(fieldname) - 1;
  memcpy(fieldname, field, len);
  fieldname[len] = 0x00;
  str = strchr(fieldname, '[');
  if (str)
    {
      *str = 0x00;
      *array = 1;
    }

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
					void **data, char translateaddr, u_char *array)
					
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
      child = revm_fieldoff_get(type, param, &off, array);
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

  child = revm_fieldoff_get(type, param, &off, array);
  if (off == REVM_INVALID_FIELD)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find field", NULL);
  *data = (char *) *data + off;

  /* Dereference pointer (struct.ptr->to.other.field) */
  if (child->isptr && *data)
    {
      if (translateaddr)
	{
	  *data = (void *) *(eresi_Addr *) *data;
	  *data = elfsh_readmema(world.curjob->curfile, (eresi_Addr) *data, NULL, 0);
	}
      else
	*data = (void *) *(u_long *) *data;
    }

  //if (!*data)
  //PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
  //	      "Cannot find dereference NULL field", NULL);

  child = revm_field_get(child, next, data, translateaddr, array);
  if (!child)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested field", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, child);
}



/** 
 * @brief Create the REVM object that is to be returned 
 */
revmobj_t	*revm_object_create(aspectype_t *type, void *data, char translateaddr, u_char array)
{
  revmobj_t	*path;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Create the revmobj and fill its handlers */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, path, sizeof(revmobj_t), NULL);
  path->root = (void *) type;

  /* In case type is unknown */
  if (!type)
    {
      path->get_data = (void *) revm_generic_getdata;
      path->set_data = (void *) revm_generic_setdata;
      goto end;
    }

  /* Lookup again in the file if we are dealing with a pointer */
  /* Pointer dereferencing is done differently for internal exprs 
     (translate = 0) and reflected exprs (translate = 1):
     sizeof(eresi_Addr) might be different than sizeof(u_long) ! 
  */
  if (type->type == ASPECT_TYPE_STR || type->isptr)
    { 
      if (translateaddr && *(eresi_Addr *) data)
	{
	  data = (void *) *(eresi_Addr *) data;
	  data = elfsh_readmema(world.curjob->curfile, (eresi_Addr) data, NULL, 0);
	}
      else if (*(u_long *) data)
	data = (void *) *(u_long *) data;
    }
  path->parent = (void *) data;

  /* In case of list or hashes */
  if (type->isptr)
    {
      if (type->type == ASPECT_TYPE_LIST || type->type == ASPECT_TYPE_HASH)
	path->get_obj  = (void *) revm_hash_getobj;
      else
	{
	  path->get_obj  = (void *) revm_long_getobj;
	  path->set_obj  = (void *) revm_long_setobj;
	}
      goto end;
    }

  /* Then put type dependent handlers */
  switch (type->type)
    {
    case ASPECT_TYPE_BYTE:
      path->get_obj  = (void *) revm_byte_getobj;
      path->set_obj  = (void *) revm_byte_setobj;
      break;
    case ASPECT_TYPE_SHORT:
      path->get_obj  = (void *) revm_short_getobj;
      path->set_obj  = (void *) revm_short_setobj;
      break;
    case ASPECT_TYPE_INT:
    case ASPECT_TYPE_OID:
      path->get_obj  = (void *) revm_int_getobj;
      path->set_obj  = (void *) revm_int_setobj;
      break;
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
      path->get_obj  = (void *) revm_addr_getobj;
      path->set_obj  = (void *) revm_addr_setobj;
      break;
    case ASPECT_TYPE_LONG:
      path->get_obj  = (void *) revm_long_getobj;
      path->set_obj  = (void *) revm_long_setobj;
      break;
    case ASPECT_TYPE_STR:
      path->get_name = (void *) revm_generic_getname;
      path->set_name = (void *) revm_generic_setname;
    default:
      break;
    }
  
 end:
  path->otype = type;
  path->immed = 0;			/* Value is not immediate */
  path->perm  = (array ? 0 : 1);	/* Free (or not) after use */

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
  u_char	array;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_EXPRS_MORE__
  fprintf(stderr, "REVMOBJ_LOOKUP_REAL (%s.%s of type %s)\n", 
	  objname, objpath, type->name);
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
    data = elfsh_readmema(world.curjob->curfile, (eresi_Addr) data,
			  (void *)&data, sizeof(void *));

  /* Get recursively the leaf type and data pointer */
  /* If the objpath is empty, its a scalar that was requested */
  if (objpath && *objpath)
    {
      type = revm_field_get(type, objpath, (void **) &data, translateaddr, &array);
      if (!type)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to lookup object", NULL);
    }

  /* Create and return the object describing the field */
  path = revm_object_create(type, data, translateaddr, array);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, path);
}


/** 
 * @brief Lookup the path for a complex typed object (using syntactic sugar) 
 */
revmobj_t	*revm_object_lookup(char *str)
{
  char		objectname[ERESI_MEANING];
  int		ret;
  aspectype_t	*type;
  revmexpr_t	*expr;
  revmobj_t	*path;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#define LOOKUP3_IDX "%41[^"REVM_SEP"]"REVM_SEP

  ret = sscanf(str, LOOKUP3_IDX, objectname);
  if (ret != 1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to resolve manual type object", NULL);

  str += strlen(objectname);
  if (str[0] && str[1])
    str++;

  expr = revm_expr_get(objectname);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested expr", NULL);
  type = expr->type;
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

eresi_Addr	revm_addr_getobj(void *data)
{
  return (*(eresi_Addr *) data);
}

eresi_Addr	revm_byte_getobj(void *data)
{
  u_char	byte;
  
  byte = *(u_char *) data;
  return ((eresi_Addr) byte);
}

eresi_Addr	revm_short_getobj(void *data)
{
  u_short	sh;
  
  sh = *(u_short *) data;
  return ((eresi_Addr) sh);
}

eresi_Addr	revm_int_getobj(void *data)
{
  u_int		ent;

  ent = *(u_int *) data;
  return ((eresi_Addr) ent);
}

eresi_Addr	revm_hash_getobj(void *data)
{
  return ((eresi_Addr) data);
}

eresi_Addr	revm_long_getobj(void *data)
{
  eresi_Addr	addr;
  u_long	l;

  l = *(long *) data;
  addr = (eresi_Addr) l;
  return (addr);
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
  long		*dst;

  dst = (long *) data;
  *dst = value;
  return (0);
}

int		revm_addr_setobj(void *data, eresi_Addr value)
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
