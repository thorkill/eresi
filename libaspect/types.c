/*
** types.c for libaspect in ERESI
** 
** The base of the unified type system
**
** Started on  Sun Jan 9 07:23:58 2007 jfv
*/
#include "libaspect.h"


/* Available types hash */
hash_t		types_hash;

/* Base types numbers, strings, and infos */
int		aspect_type_nbr       = ASPECT_TYPE_BASENUM;
typeinfo_t	*aspect_typeinfo      = NULL;
typeinfo_t	aspect_typeinfo_base[ASPECT_TYPE_BASENUM] = 
  {
    {ASPECT_TYPENAME_UNKNOW , 0			},
    {ASPECT_TYPENAME_RAW    , 0			},
    {ASPECT_TYPENAME_BYTE   , sizeof(u_char)	},
    {ASPECT_TYPENAME_STR    , sizeof(u_long)	},
    {ASPECT_TYPENAME_SHORT  , sizeof(u_short)	},
    {ASPECT_TYPENAME_INT    , sizeof(u_int)	},
    {ASPECT_TYPENAME_LONG   , sizeof(u_long)	},
    {ASPECT_TYPENAME_DADDR  , sizeof(u_long)	},
    {ASPECT_TYPENAME_CADDR  , sizeof(u_long)	},
    {ASPECT_TYPENAME_VECT   , sizeof(vector_t)	},
    {ASPECT_TYPENAME_HASH   , sizeof(hash_t)	},
  };


/* Copy the parameter type by change its offset */
static aspectype_t	*aspect_type_copy(aspectype_t	*type, 
					  unsigned int	off, 
					  u_char	isptr, 
					  u_int		elemnbr, 
					  char		*fieldname,
					  u_int		*dims)
{
  aspectype_t		*newtype;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 newtype, sizeof(aspectype_t), NULL);
  memcpy(newtype, type, sizeof(aspectype_t));
  newtype->off        = off;
  newtype->isptr      = isptr;
  newtype->dimnbr     = elemnbr;
  newtype->fieldname  = fieldname;
  newtype->elemnbr    = dims;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newtype);
}



/* Add a field to a type */
static int		aspect_type_addfield(aspectype_t *parent, 
					     aspectype_t *field)
{
  aspectype_t		*next;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!parent || !field)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", 0);
  if (!parent->childs)
    parent->childs = field;
  else 
    {
      for (next = parent->childs; next->next; next = next->next);
      next->next = field;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
}



/* Find the number of dimensions for a field */
static u_int	*aspect_type_getdims(char *typename, int *dimnbr)
{
  char		*idxname;
  char		*idxend;
  u_int		*dims;
  char		*realtypename;
  int		idx;
  int		curidx;
  u_int		sz;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First count the number of dimensions */
  for (idx = 0, realtypename = typename; *typename; 
       typename = idxend + 1)
    {
      idxname = strchr(typename, '[');
      if (idxname)
	{
	  idxend = strchr(idxname + 1, ']');
	  if (!idxend)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Invalid array dimensions", NULL);
	  idx++;
	}
      else
	break;
    }

  *dimnbr = idx;

  /* Then allocate the array */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 dims, idx * sizeof(int), NULL);

  /* Fill array with fetched indexes */
  for (curidx = 0, typename = realtypename; 
       curidx < idx; 
       typename = idxend + 1, curidx++)
    {
      idxname  = strchr(typename, '[');
      *idxname = 0x00;
      idxend   = strchr(idxname + 1, ']');
      sz       = atoi(idxname + 1);
      if (!sz)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Invalid array element number", NULL);
      dims[curidx] = sz;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dims);
}


/* Create a new type */
static aspectype_t	*aspect_type_create(char *label, 
					    char **fields, 
					    u_int fieldnbr)
{
  aspectype_t		*newtype;      
  aspectype_t		*childtype;
  aspectype_t		*copy;
  int			index;
  char			*fieldname;
  char			*typename;
  char			*fieldsz;
  u_int			curoff, off;
  char			isptr;
  int			dimnbr;
  u_int			*dims;
  u_int			idx;
  u_int			size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!label || !fields || !fieldnbr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);
  if (hash_get(&types_hash, label))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot create type : already exists", NULL);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 newtype, sizeof(aspectype_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 newtype->childs, sizeof(hash_t), NULL);

  newtype->childs = NULL;
  newtype->name   = aproxy_strdup(label);
 
    
  /* Add fields to types */
  for (dims = NULL, off = dimnbr = isptr = curoff = index = 0; 
       index < fieldnbr; 
       index++, off = dimnbr = isptr = 0, dims = NULL)
    {
      /* Field name */
      fieldname = fields[index];

      /* Typename */
      typename  = strchr(fields[index], ':');
      if (typename)
	*typename++ = 0x00;

      /* Size field */
      fieldsz = strchr(fields[index], '%');
      if (fieldsz)
	*fieldsz++ = 0x00;

      /* Either you provide the size or the typename */
      if (!*fieldname || (!typename && !fieldsz) || 
	  (typename && fieldsz) || (typename && !*typename) || 
	  (fieldsz && (!*fieldsz || !atoi(fieldsz))))
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__, newtype->name);
	  XFREE(__FILE__, __FUNCTION__, __LINE__, newtype);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			    "Invalid type structure", NULL);
	}

      /* Support raw types determined by their size */
      if (!typename) 
	{
	  off = atoi(fieldsz);
	  XALLOC(__FILE__, __FUNCTION__, __LINE__,
		 childtype, sizeof(aspectype_t), NULL);
	  childtype->type = ASPECT_TYPE_RAW;
	  childtype->size = off;
	  childtype->name = aspect_typeinfo[childtype->type].name;
	}

      /* Deals with incomplete types */
      else
	{
	  /* Pointer types */
	  if (typename[0] == '*')
	    {
	      isptr = 1;
	      typename++;
	    }

	  /* Array type */
	  dims = aspect_type_getdims(typename, &dimnbr);
	  if (dimnbr < 0)
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__,newtype->name);
	      XFREE(__FILE__, __FUNCTION__, __LINE__,newtype);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				"Invalid array structure", NULL);
	    }

	  /* Lookup field type */
	  childtype = hash_get(&types_hash, typename);
	  if (!childtype)
	    {
	      /* If we have a non-pointer recursive type, 
		 make it a pointer */
	      if (!strcmp(label, typename))
		{
		  isptr = 1;
		  childtype = newtype;
		  off = sizeof(unsigned long);
		}
	      
	      /* If we point on a unknown type, mark is as unknown */
	      else if (isptr)
		{
		  XALLOC(__FILE__, __FUNCTION__, __LINE__,
			 childtype, sizeof(aspectype_t), NULL);
		  childtype->type = ASPECT_TYPE_UNKNOW;
		  childtype->name = aproxy_strdup(typename);
 
		}
	      else
		{
		  XFREE(__FILE__, __FUNCTION__, __LINE__, newtype->name);
		  XFREE(__FILE__, __FUNCTION__, __LINE__, newtype);
		  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				    "Invalid type structure", NULL);
		}
	    }
	  else
	    off = (isptr ? sizeof(unsigned long) : childtype->size);
	}
      
      /* Copy an existing base type for this child and 
	 change type offset */
      copy    = aspect_type_copy(childtype, curoff, isptr, 
				 dimnbr, fieldname, dims);
      for (size = off, idx = 0; idx < dimnbr; idx++)
	size *= dims[idx];
      curoff += size;
      aspect_type_addfield(newtype, copy);
    }
  
  /* Add type to global type hash table and return success */
  newtype->size = curoff;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newtype);  
}


/* Interface for type creation */
int		aspect_type_register(char *label, 
				     char **fields, 
				     u_int fieldnbr)
{
  aspectype_t	*ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = aspect_type_create(label, fields, fieldnbr);
  if (!ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid type declaration", -1);
  hash_add(&types_hash, label, ret);

  /* The type ID is incremented here */
  aspect_type_nbr++;
  XREALLOC(__FILE__, __FUNCTION__, __LINE__,
	   aspect_typeinfo, aspect_typeinfo, aspect_type_nbr, -1);
  aspect_typeinfo[aspect_type_nbr - 1].name = label;
  aspect_typeinfo[aspect_type_nbr - 1].size = ret->size; 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Create a type with a single field */
static int	aspect_basetype_create(u_int type, typeinfo_t *info)
{
  aspectype_t	*newtype;
  char		hashname[BUFSIZ];
  hash_t	*newhash;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 newtype, sizeof(aspectype_t), -1);
  newtype->type = type;
  newtype->size = info->size;
  newtype->name = info->name; 
  hash_add(&types_hash, info->name, newtype);

  /* Create the hash table for objects of that type */
  snprintf(hashname, sizeof(hashname), "type_%s", info->name);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 newhash, sizeof(hash_t), -1);
  hash_init(newhash, aproxy_strdup(hashname), 11, ASPECT_TYPE_UNKNOW);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}


/* Create all simple types */
int		aspect_basetypes_create()
{
  int		index;
  u_int		basesize;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  basesize = ASPECT_TYPE_BASENUM * (sizeof(char*) + sizeof(int));
  XALLOC(__FILE__, __FUNCTION__, __LINE__, aspect_typeinfo, basesize, -1);
  memcpy(aspect_typeinfo, aspect_typeinfo_base, basesize);
  for (index = 1; index < ASPECT_TYPE_BASENUM; index++)
    aspect_basetype_create(index, aspect_typeinfo + index);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}

/* Retreive the ascii name of a type */
char		*aspect_typename_get(u_int type)
{
  if (type >= aspect_type_nbr)
    return (NULL);
  return (aspect_typeinfo[type].name);
}
