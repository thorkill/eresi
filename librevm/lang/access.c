/*
** access.c : Various lookup functions for generic types
**
** Started Jan 23 2007 23:39:51 mayhem
*/
#include "revm.h"


/* Return offset given field name */
revmtype_t	*vm_fieldoff_get(revmtype_t *parent, char *field, u_int *off)
{
  revmtype_t	*child;

  for (child = parent->childs; child; child = child->next)
    if (!strcmp(child->name, field))
      {
	*off = child->off;
	return (child);
      }
  *off = REVM_INVALID_FIELD;
  return (NULL);
}

/* Recursive function to lookup data from its type path */
static revmtype_t	*vm_field_get(revmtype_t *type, char *param, void **data)
{
  char			*str;
  char			*next;
  unsigned int		off;
  revmtype_t		*child;

  str = strchr(param, *REVM_SEP);

  /* This is the leaf field */
  if (!str)
    {

      child = vm_fieldoff_get(type, param, &off);
      if (off == REVM_INVALID_FIELD)
	return (NULL);
      *data = (char *) *data + off;

      return (child);
    }

  /* This is a non-terminal child field */
  *str = 0x00;
  next = str + 1;

  child = vm_fieldoff_get(type, param, &off);
  if (off == REVM_INVALID_FIELD)
    return (NULL);
  *data = (char *) *data + off;

  return (vm_field_get(child, next, data));
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
  revmtype_t	*type;
  void		*data;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = sscanf(str, "%s.%s[%s]", filename, typename, objectname);
  if (ret != 3)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to resolve manual type object", NULL);
  str += strlen(filename) + strlen(typename) + strlen(objectname) + 3;
  
  /* Get parent objects */
  obj = vm_lookup_file(filename);
  if (obj == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested file object", NULL);
  type = hash_get(&types_hash, typename);
  if (!type)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested type", NULL);
  snprintf(hashname, sizeof(hashname), "type_%s", typename);
  typehash = hash_get(hash_hash, hashname);
  if (!typehash)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested type map", NULL);
  data = hash_get(typehash, objectname);
  if (!data)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested data object", NULL);

  /* Get recursively the leaf type and data pointer */
  type = vm_field_get(type, str, (void **) &data);
  
  // XXX: FIXME: Construct the revmobj_t with correct set/get handlers
  // getname(o->root, o->parent) for GET strings (return char*)
  // setname(o->root, o->parent, char*str) for SET strings
  // getobj(o->parent) for GET others (return plain value)
  // setobj(o->parent, plainval) for SET plain values
  // getdata(o->parent, o->off, o->szelm) -> GET void* data
  // setdata(o->parent, o->off, data, sz, szelm) -> write data


  /* Default error case */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to create revmobj_t handlers", NULL);
}

