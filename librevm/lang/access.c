/*
** access.c : Implementation of scripting for language types
**
** Started Jan 23 2007 23:39:51 mayhem
*/
#include "revm.h"


/* Get the buffered address from the real virtual address */
void		*vm_get_raw(void *addr)
{
  elfshsect_t	*sect;
  elfsh_SAddr	offset;

  sect = elfsh_get_parent_section(world.curjob->current, 
				  (elfsh_Addr) addr, &offset);
  if (!sect)
    return (NULL);
  if (elfsh_is_debug_mode())
    {
      if (!elfsh_section_is_runtime(sect))
	return ((void *) (sect->parent->rhdr.base + sect->shdr->sh_addr + offset));
      else if (!sect->shdr->sh_addr)
	return ((void *) (char *) sect->data + offset);
      else
	return ((void *) (sect->shdr->sh_addr + offset));
    }
  else
    return ((void *) (char *) sect->data + offset);

}


/* Return the requested projections in case of an array */
int		vm_arrayoff_get(char *field, u_int elmsize, 
				u_int dimnbr, u_int *dims)
{
  char		*strindex;
  char		*endindex;
  int		offset;
  int		index;
  int		iter;
  int		liter;
  u_int		*dimoff;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  dimoff = alloca(dimnbr * sizeof(int));

  /* Compute offset for each dimension in the first pass */
  for (iter = 0; 
       field && *field && iter < dimnbr; 
       field = endindex + 1, iter++)
    {
      strindex = strchr(field, '[');
      if (strindex)
	{
	  *strindex = 0x00;
	  endindex = strchr(strindex + 1, ']');
	  if (!endindex)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Invalid array syntax", -1);
	  *endindex = 0x00;
	  index = atoi(strindex + 1);
	  if (index >= dims[iter] || (strindex + 1) == endindex)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Invalid array index", -1);

	  /* Example [2][3][2] = (2 * dim2 * dim3 + 3 * dim3 + 2) * sizeof(int) */
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




/* Return offset given field name */
aspectype_t	*vm_fieldoff_get(aspectype_t *parent, char *field, u_int *off)
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
	index = vm_arrayoff_get(field, child->size, child->dimnbr, child->elemnbr);
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


/* Recursive function to lookup data from its type path */
static aspectype_t	*vm_field_get(aspectype_t *type, char *param, void **data)
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
      child = vm_fieldoff_get(type, param, &off);
      if (off == REVM_INVALID_FIELD)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot find terminal field", NULL);
      *data = (char *) *data + off;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, child);
    }

  /* This is a non-terminal child field */
  *str = 0x00;
  next = str + 1;

  child = vm_fieldoff_get(type, param, &off);
  if (off == REVM_INVALID_FIELD)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find field", NULL);
  *data = (char *) *data + off;

  /* Dereference pointers */
  if (child->isptr)
    *data = (void *) *(elfsh_Addr *) *data;

  child = vm_field_get(child, next, data);
  if (!child)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested field", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, child);
}


/* Lookup the path for a complex typed object */
revmobj_t	*vm_revmobj_lookup(char *str)
{
  char		filename[ELFSH_MEANING];
  char		typename[ELFSH_MEANING];
  char		hashname[ELFSH_MEANING];
  char		objectname[ELFSH_MEANING];
  hash_t	*typehash;
  int		ret;
  elfshobj_t	*obj;
  aspectype_t	*type;
  revmvar_t	*var;
  void		*data;
  revmobj_t	*path;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#define LOOKUP3_IDX "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^]]]"

  ret = sscanf(str, LOOKUP3_IDX, filename, typename, objectname);
  if (ret != 3)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to resolve manual type object", NULL);
  str += strlen(filename) + strlen(typename) + strlen(objectname) + 3;
  if (!str[0] || !str[1])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid truncated field name", NULL);
  str++;

  /* Get parent objects */
  obj = vm_lookup_file(filename);
  if (obj == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object", NULL);
  type = hash_get(&types_hash, typename);
  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested type", NULL);
  snprintf(hashname, sizeof(hashname), "type_%s", typename);
  typehash = hash_get(hash_hash, hashname);
  if (!typehash)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested type map", NULL);
  var = hash_get(typehash, objectname);
  if (!data)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested data object", NULL);
  data = (void *) var->addr;
  data = vm_get_raw(data);

  /* Get recursively the leaf type and data pointer */
  type = vm_field_get(type, str, (void **) &data);
  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to lookup object", NULL);

  /* Create the revmobj and fill its handlers */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,path, sizeof(revmobj_t), NULL);
  path->root     = (void *) type;

  /* Lookup again in the file if we are dealing with a pointer */
  if (type->type == ASPECT_TYPE_STR || type->isptr)
    {
      data = (void *) *(elfsh_Addr *) data;
      data = vm_get_raw(data);
    }

  /* Fill type specific object handlers */
  path->parent   = (void *) data;
  if (type->type == ASPECT_TYPE_STR)
    {
      path->get_name = (void *) vm_generic_getname;
      path->set_name = (void *) vm_generic_setname;
    }
  if (type->type == ASPECT_TYPE_RAW)
    {
      path->get_data = (void *) vm_generic_getdata;
      path->set_data = (void *) vm_generic_setdata;
    }
  path->get_obj  = (void *) vm_generic_getobj;

  /* This handler is size dependant */
  switch (type->type)
    {
    case ASPECT_TYPE_BYTE:
      path->set_obj  = (void *) vm_byte_setobj;
      break;
    case ASPECT_TYPE_SHORT:
      path->set_obj  = (void *) vm_short_setobj;
      break;
    case ASPECT_TYPE_INT:
      path->set_obj  = (void *) vm_int_setobj;
      break;
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
      path->set_obj  = (void *) vm_long_setobj;
      break;
    default:
      break;
    }
  path->type     = type->type;
  path->immed    = 0;		/* Value is not immediate */
  path->perm	 = 1;		/* Do not free after use  */

  /* Success */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, path);
}



/**************** Generic handlers *********************************/



char		*vm_generic_getname(void *type, void *data)
{
  return (data);
}

int		vm_generic_setname(void *type, void *data, void *newdata)
{
  strcpy(data, newdata);
  return (0);
}

elfsh_Addr	vm_generic_getobj(void *data)
{
  return (*(elfsh_Addr *) data);
}

int		vm_byte_setobj(void *data, elfsh_Addr value)
{
  unsigned char	*byte;

  byte  = (unsigned char *) data;
  *byte = (unsigned char) value;
  return (0);
}

int		vm_short_setobj(void *data, elfsh_Addr value)
{
  u_short	*half;

  half  = (u_short *) data;
  *half = (u_short) value;
  return (0);
}

int		vm_int_setobj(void *data, elfsh_Addr value)
{
  unsigned int	*val;

  val  = (unsigned int *) data;
  *val = (unsigned int) value;
  return (0);
}

int		vm_long_setobj(void *data, elfsh_Addr value)
{
  elfsh_Addr	*dst;

  dst = (elfsh_Addr *) data;
  *dst = value;
  return (0);
}

char		*vm_generic_getdata(void *data, int off, int sizelm)
{
  return ((char *) data + (off * sizelm));
}

int		vm_generic_setdata(void *data, int off, void *newdata, 
				   int size, int sizelm)
{
  memcpy((char *) data + (off * sizelm), newdata, size);
  return (0);
}
