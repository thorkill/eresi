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

  /* Check for pointers */
  if (child->isptr)
    *data = (void *) *(elfsh_Addr *) *data;

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
  revmobj_t	*path;
  
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
  if (!type)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to lookup object", NULL);

  XALLOC(path, sizeof(revmobj_t), NULL);
  path->root     = (void *) type;
  path->parent   = (void *) data;
  path->get_name = (void *) vm_generic_getname;
  path->set_name = (void *) vm_generic_setname;
  path->get_obj  = (void *) vm_generic_getobj;
  path->set_obj  = (void *) vm_generic_setobj;
  path->get_data = (void *) vm_generic_getdata;
  path->set_data = (void *) vm_generic_setdata;

  path->type     = type->type;
  path->immed    = 0;		/* Value is not immediate */
  path->perm	 = 1;		/* Do not free after use  */

  /* Success */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, path);
}



/* Generic handlers */
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

int		vm_generic_setobj(void *data, elfsh_Addr value)
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
