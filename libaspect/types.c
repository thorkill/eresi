/**
* @file libaspect/types.c
** @ingroup libaspect
** @brief The base of the unified type system.
**
** Started on  Sun Jan 9 07:23:58 2007 jfv
** $Id$
*/
#include "libaspect.h"


/**
 * @brief Available types hash 
 */
hash_t		types_hash;

/* Base types numbers, strings, and infos */
/* Type Unknown is always at index 0 */
int		aspect_type_nbr       = ASPECT_TYPE_BASENUM;
typeinfo_t	*aspect_typeinfo      = NULL;
typeinfo_t	aspect_typeinfo_base[ASPECT_TYPE_BASENUM] = 
  {
    {ASPECT_TYPENAME_UNKNOW  , 0	      	},
    {ASPECT_TYPENAME_RAW     , 0	  	},
    {ASPECT_TYPENAME_BYTE    , sizeof(u_char)	},
    {ASPECT_TYPENAME_STR     , sizeof(u_long) },
    {ASPECT_TYPENAME_SHORT   , sizeof(u_short)	},
    {ASPECT_TYPENAME_INT     , sizeof(u_int)	},
    {ASPECT_TYPENAME_LONG    , sizeof(u_long) },
    {ASPECT_TYPENAME_DADDR   , sizeof(eresi_Addr) },
    {ASPECT_TYPENAME_CADDR   , sizeof(eresi_Addr) },
    {ASPECT_TYPENAME_BIT     , sizeof(u_char)	},
    {ASPECT_TYPENAME_OID     , sizeof(u_int)    },
    {ASPECT_TYPENAME_VECT    , sizeof(vector_t)	},
    {ASPECT_TYPENAME_HASH    , sizeof(hash_t)  	},
    {ASPECT_TYPENAME_LIST    , sizeof(list_t)  	},
    {ASPECT_TYPENAME_EXPR    , 28		}, /* XXX: should be sizeof(revmexpr_t) */
    {ASPECT_TYPENAME_BLOC    , 13		}, /* XXX: should be sizeof(mjrblock_t) */
    {ASPECT_TYPENAME_FUNC    , 110		}, /* XXX: should be sizeof(mjrfunc_t) */
    {ASPECT_TYPENAME_LINK    , 6                }, /* XXX: should be sizeof(mjrlink_t) */
  };

/**
 * @brief Indicate if a type is simple (1) or not (0)
 * @param typeid Type identifier to be checked 
 * @return 1 if type is simple, 0 if not
 */
int			aspect_type_simple(int typeid)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (typeid != ASPECT_TYPE_UNKNOW && typeid < ASPECT_TYPE_SIMPLENUM)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * @brief Copy the structure representing a field data type for creating a new meta-type instance
 * @param type Type structure for the field data type 
 * @param off Offset in parent data type
 * @param isptr Indicate if field data type is a pointer
 * @param elemnbr Element numbers (if typing an array)
 * @param fieldname Name for typed field
 * @param dims Dimension array for type
 * @return Type structure derived from input information
 */
aspectype_t		*aspect_type_copy(aspectype_t	*type, 
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
  newtype->fieldname  = strdup(fieldname);
  newtype->elemnbr    = dims;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newtype);
}


/**
 * @brief Copy the structure representing a data type and change its name to create a new meta-type
 * @param type Type structure to be copied
 * @param name Type name to be copied
 * @param fieldshash Hash table of fields for this type (if structure type)
 * @param curdepth Number of pointer indirections since start of copy
 * @param maxdepth Maximum number of pointer indirections for whole copy
 * @return Copied meta-type structure
 */
aspectype_t		*aspect_type_copy_by_name(aspectype_t   *type, 
						  char		*name, 
						  hash_t	*fieldshash,
						  u_int		curdepth,
						  u_int		maxdepth)
{
  aspectype_t		*newtype;
  aspectype_t		*result;
  aspectype_t		*next;
  aspectype_t		*prev;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (curdepth > maxdepth)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid depth parameters", NULL);

  /* Allocate and name the new type */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 newtype, sizeof(aspectype_t), NULL);
  memcpy(newtype, type, sizeof(aspectype_t));
  if (name)
    newtype->name = strdup(name);
  if (fieldshash && newtype->fieldname)
    hash_add(fieldshash, strdup(newtype->fieldname), (void *) 1);
  prev = result = newtype;

  /* Here check if newtype->childs is a pointer */
  if (newtype->childs)
    {
      if (!newtype->isptr || curdepth != maxdepth || !maxdepth)
	{
	  if (newtype->isptr)
	    curdepth++;
	  newtype->childs = aspect_type_copy_by_name(newtype->childs, NULL, 
						     (!newtype->fieldname ? fieldshash : NULL),
						     curdepth, maxdepth);
	  if (newtype->isptr)
	    curdepth--;
	}
      else
	newtype->childs = NULL;
    }

  /* Copy all the field types, if we are dealing with a record type */
  for (next = newtype->next; next; next = next->next)
    {

      /* Copy the type structure */
      XALLOC(__FILE__, __FUNCTION__, __LINE__,
	     newtype, sizeof(aspectype_t), NULL);
      memcpy(newtype, next, sizeof(aspectype_t));

      /* Set this field name as used to avoid doubles */
      if (fieldshash)
	hash_add(fieldshash, strdup(newtype->fieldname), (void *) 1);

      /* Here check if next->childs is a pointer */
      if (next->childs)
	{
	  if (!next->childs->isptr || curdepth != maxdepth || !maxdepth)
	    {
	      if (next->childs->isptr)
		curdepth++;
	      newtype->childs = aspect_type_copy_by_name(next->childs, 
							 NULL, NULL, 
							 curdepth, maxdepth);
	      if (next->childs->isptr)
		curdepth--;
	    }
	  else
	    next->childs = NULL;
	}

      /* Now next type in structure */
      prev->next = newtype;
      prev = newtype;
    }

  /* Return copied root type as a result */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, result);
}


/** 
 * @brief Add a field to a meta-type
 * @param parent Parent data type 
 * @param field Field data type to add to parent
 * @return 0 on succes and -1 on error
 */
int		aspect_type_addfield(aspectype_t *parent, 
				     aspectype_t *field)
{
  volatile aspectype_t		*next;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!parent || !field)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);
  if (!parent->childs)
    parent->childs = field;
  else 
    {
      for (next = parent->childs; next->next; next = next->next);
      next->next = field;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * @brief Find the number of dimensions for a field 
 * @param typename Complete type name string (can be array)
 * @param dimnbr Pointer on dimension number integer to be filled
 * @return NULL on error or dimension array on succesfull allocation
*/
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


/**
 * @brief Find the size of an union type 
 * @param utype Union type whose size is to be infered
 * @return Total size for union type
 */
int			aspect_type_find_union_size(aspectype_t *utype)
{
  int			biggest = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!utype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input type", 0);
  if (!utype->next && utype->childs)
    utype = utype->childs;
  while (utype)
    {
      if (utype->size > biggest)
	biggest = utype->size;
      utype = utype->next;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, biggest);
}



/**
 * @brief Create a new (meta description) type
 * @param isunion 1 if type to be created is a union
 * @param label String name for new type
 * @param fields List of fields for type
 * @param fieldnbr Number of fields for type
 * @return Created type structure
 */
aspectype_t		*aspect_type_create(u_char isunion,
					    char *label, 
					    char **fields, 
					    u_int fieldnbr)
{
  aspectype_t		*newtype;      
  aspectype_t		*childtype;
  aspectype_t		*copy;
  aspectype_t		*supertype;
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
  hash_t		fields_hash;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!label || !fields || !fieldnbr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", NULL);

  /* Subtyping was specified */
  supertype = NULL;
  typename = strstr(label, "::");
  if (typename)
    {
      *typename = 0x00;
      typename += 2;
      supertype = aspect_type_get_by_name(typename);
      if (!supertype)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid record derivation", NULL); 
    }
  
  /* Check that the created type name is not existing already */
  /* It is authorized to update/change some specific types: bloc, func, vector, hash, list .. */
  newtype = hash_get(&types_hash, label);
  if (newtype && (newtype->type < ASPECT_TYPE_CORENUM || newtype->type >= ASPECT_TYPE_BASENUM))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot create type : name already exists", NULL);

  /* Just remember the name of fields we have already created */
  bzero(&fields_hash, sizeof(hash_t));
  hash_init(&fields_hash, "localfields", 10, ASPECT_TYPE_UNKNOW);
  
  /* Allocate the new type structure */
  if (!supertype)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__,
	     newtype, sizeof(aspectype_t), NULL);
      XALLOC(__FILE__, __FUNCTION__, __LINE__,
	     newtype->childs, sizeof(hash_t), NULL);
      newtype->childs = NULL;
      newtype->name = strdup(label);   
      curoff = 0;
    }
  else
    {
      newtype = aspect_type_copy_by_name(supertype, label, &fields_hash, 0, ASPECT_TYPE_MAXPTRDEPTH);
      curoff = newtype->size;
    }
  
  /* Add fields to types */
  for (dims = NULL, off = dimnbr = isptr = index = 0; 
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
      if (!*fieldname || hash_get(&fields_hash, fieldname) || 
	  (!typename && !fieldsz) || (typename && fieldsz) || 
	  (typename && !*typename) || (fieldsz && (!*fieldsz || !atoi(fieldsz))))
	  
	{
	  hash_destroy(&fields_hash);
	  XFREE(__FILE__, __FUNCTION__, __LINE__, newtype->name);
	  XFREE(__FILE__, __FUNCTION__, __LINE__, newtype);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			    "Invalid use of fieldname or typename", NULL);
	}
      hash_add(&fields_hash, strdup(fieldname), (void *) 1);

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
	  isptr = 0;
	  while (typename[0] == '*')
	    {
	      isptr += 1;
	      typename++;
	    }

	  /* Array type */
	  dims = aspect_type_getdims(typename, &dimnbr);
	  if (dimnbr < 0)
	    {
	      hash_destroy(&fields_hash);
	      XFREE(__FILE__, __FUNCTION__, __LINE__,newtype->name);
	      XFREE(__FILE__, __FUNCTION__, __LINE__,newtype);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				"Invalid array dimensions", NULL);
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
		  off = sizeof(u_long);
		}
	      
	      /* If we point on a unknown type, mark is as unknown */
	      else if (isptr)
		{
		  XALLOC(__FILE__, __FUNCTION__, __LINE__,
			 childtype, sizeof(aspectype_t), NULL);
		  childtype->type = ASPECT_TYPE_UNKNOW;
		  childtype->name = strdup(typename);
 
		}
	      else
		{
		  hash_destroy(&fields_hash);
		  XFREE(__FILE__, __FUNCTION__, __LINE__, newtype->name);
		  XFREE(__FILE__, __FUNCTION__, __LINE__, newtype);
		  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			       "Invalid type structure", NULL);
		}
	    }
	  else
	    off = (isptr ? sizeof(u_long) : childtype->size);
	}
      
      /* Copy an existing base type for this child and change type offset */
      copy = aspect_type_copy(childtype, (isunion ? 0 : curoff), isptr, 
			      dimnbr, fieldname, dims);
      for (size = off, idx = 0; dims != NULL && idx < dimnbr; idx++)
	size *= dims[idx];

      /* Compute the correct size for bitfields */
      if (childtype->type == ASPECT_TYPE_BIT)
	{
	  size /= 8;
	  size++;
	}

      curoff += size;
      aspect_type_addfield(newtype, copy);
    }
  
  /* Add type to global type hash table and return success */
  newtype->size = (isunion ? aspect_type_find_union_size(newtype) : curoff);
  hash_destroy(&fields_hash);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newtype);  
}


/**
 * @brief The real type registering code 
 * @param label Name for type to be registered
 * @param ntype Type structure to be registered
 * @return 0 on success and -1 on error
 */
int		aspect_type_register_real(char	      *label, 
					  aspectype_t *ntype)
{
  aspectype_t	*update;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We do a type update of one of the base type */
  update = hash_get(&types_hash, label);
  if (update)
    {
      ntype->type = update->type;
      hash_set(&types_hash, label, ntype);
      aspect_typeinfo[ntype->type].size = ntype->size;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* A real new type incrementing the last type id */
  hash_add(&types_hash, label, ntype);
  aspect_type_nbr++;
  XREALLOC(__FILE__, __FUNCTION__, __LINE__,
	   aspect_typeinfo, aspect_typeinfo, 
	   sizeof(typeinfo_t) * aspect_type_nbr, -1);
  aspect_typeinfo[aspect_type_nbr - 1].name = label;
  aspect_typeinfo[aspect_type_nbr - 1].size = ntype->size; 
  ntype->type = aspect_type_nbr - 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Wrapper for easy type creation and registration
 * @param isunion 1 if registered type is a union
 * @param label Name of registered type
 * @param fields Array of field names for registered type
 * @param fieldnbr Number of fields for registered type
 * @return 0 on success and -1 on error
 */
int		aspect_type_register(u_char isunion,
				     char *label, 
				     char **fields, 
				     u_int fieldnbr)
{
  aspectype_t	*ret;
  int 		iret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = aspect_type_create(isunion, label, fields, fieldnbr);

  if (!ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid type declaration", -1);

  iret = aspect_type_register_real(label, ret);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, iret);
}


/** 
 * @brief Create a new type with a single field (internal function)
 * @param type Type identifier to be created
 * @param info Internal typeinfo structure related to newly created type
 * @return 0 on success and -1 on error
 */
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
  hash_init(newhash, strdup(hashname), 11, ASPECT_TYPE_UNKNOW);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}


/**
 * @brief Create all simple types 
 * @return 0 on success and -1 on error
 */
int		aspect_basetypes_create()
{
  int		index;
  u_int		basesize;
  static u_int	done = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (done)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  done = 1;
  basesize = ASPECT_TYPE_BASENUM * sizeof(typeinfo_t);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, aspect_typeinfo, basesize, -1);
  memcpy(aspect_typeinfo, aspect_typeinfo_base, basesize);
  for (index = 1; index < ASPECT_TYPE_BASENUM; index++)
    aspect_basetype_create(index, aspect_typeinfo + index);
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}

/**
 * @brief Get field type from field name and parent type
 * @param parent Parent type structure
 * @param name Field name to lookup
 * @return Type for looked-up field
 */
aspectype_t	*aspect_type_get_child(aspectype_t *parent, char *name)
{
  aspectype_t	*cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (cur = parent->childs; cur; cur = cur->next)
    if (!strcmp(cur->fieldname, name))
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cur);  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unknown child name", NULL);
}

/**
 * @brief Get a type by its type id 
 * @param id Type identifier to be found
 * @return Structure for matching type
 */
aspectype_t	*aspect_type_get_by_id(unsigned int id)
{
  aspectype_t	*type;
  typeinfo_t    info;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (id >= aspect_type_nbr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid type ID", NULL);
  info = aspect_typeinfo[id];
  type = hash_get(&types_hash, info.name);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);  
}

/** 
 * @brief Get a type by its type id 
 * @param name Type name to be found
 * @return Structure for matching type
 */
aspectype_t	*aspect_type_get_by_name(char *name)
{
  aspectype_t	*type;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  type = hash_get(&types_hash, name);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);  
}



/**
 * @brief Return the list of base types 
 * @param nbr Integer pointer that will be filled with the number of base types
 * @return Pointer on array of internal Typeinfo structures
 */
typeinfo_t	*aspect_basetype_get(unsigned int *nbr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (nbr)
    *nbr = ASPECT_TYPE_BASENUM;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, aspect_typeinfo_base);  
}


/**
 * @brief Create and register a new simple type 
 * @param name Name for new base type
 * @param size Total size for new base type
 * @return -1 on error and 0 on success
 */
int		aspect_basetype_register(char *name, u_int size)
{
  typeinfo_t	info;
  aspectype_t	*exist;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  exist = (aspectype_t *) hash_get(&types_hash, name);

  if (exist)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Type destination already exist", -1);      

  info.name = name;
  info.size = size;

  aspect_basetype_create(aspect_type_nbr, &info);

  /* The type ID is incremented here */
  aspect_type_nbr++;
  XREALLOC(__FILE__, __FUNCTION__, __LINE__,
	   aspect_typeinfo, aspect_typeinfo, 
	   sizeof(typeinfo_t) * aspect_type_nbr, -1);
  aspect_typeinfo[aspect_type_nbr - 1].name = name;
  aspect_typeinfo[aspect_type_nbr - 1].size = size; 

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}

/** 
 * @brief Retreive the ascii name of a type 
 * @param type Requested type identifier
 * @return Name associated to type identifier
 */
char		*aspect_typename_get(u_int type)
{
  if (type >= aspect_type_nbr)
    return (NULL);
  return (aspect_typeinfo[type].name);
}

/**
 * @brief Retreive the size (in bytes) of a type 
 * @param type Requested type identifier
 * @return Size of requested type
 */
u_int		aspect_typesize_get(u_int type)
{
  if (type >= aspect_type_nbr)
    return (0);
  return (aspect_typeinfo[type].size);
}

