/**
 * @defgroup librevm_api librevm API
 */
/*
** @file atomic.c
** @ingroup librevm_api
** @brief Implement the API for atomic arithmetic and logic ERESI commands.
**
** Started on  Sun Feb  9 22:43:34 2003 jfv
**
** $Id: atomic.c,v 1.18 2008-02-16 12:32:27 thor Exp $
**
*/
#include "revm.h"


/* Convert and see if interaction is possible between two expressions */
int		revm_nextconds_atomics(revmexpr_t *o1, revmexpr_t *o2)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Lazy typing in action */
  if (!o2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Source parameter undefined", -1);

  if (!o1->type)
    revm_convert_object(o1, o2->type->type);
  else if (o1->type->type != o2->type->type)
    if (revm_convert_object(o2, o1->type->type) < 0)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "SET parameters type are not compatible", -1);
  
  /* Make sure we dont want to write in a constant */
  if (o1->value && o1->value->immed && !o1->value->perm)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Destination parameter must not be a constant", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Preconditions on atomic operations set (only called by cmd_set) */
int                     revm_preconds_atomics(revmexpr_t **o1, revmexpr_t **o2)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  *o1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  *o2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!*o1 || !*o2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup a parameter", -1);
  if (revm_nextconds_atomics(*o1, *o2) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Will not be able to compute with such types", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Preconditions on arithmetic operations */
/* Used by add, sub, mul, div, mod */
int			revm_arithmetics(revmexpr_t *e1, revmexpr_t *e2, unsigned char op)
{
  eresi_Addr		src;
  eresi_Addr		dst;
  char			buf[BUFSIZ];
  revmobj_t		*res;
  revmobj_t		*o1;
  revmobj_t		*o2;
  revmexpr_t		*last;
  int			oldtype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!e1 || !e2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup a parameter", -1);
  o1 = e1->value;
  o2 = e2->value;
  if (!o1 || !o2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Incompatible parameter type", -1);

  /* First check if we write in a constant */
  if (o1->immed && !o1->perm)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Destination parameter must not be a constant", -1);

  /* Preliminary checks */
  oldtype = o1->otype->type;
  if (o1->otype->type == ASPECT_TYPE_UNKNOW && o1->perm)
    o1->otype = aspect_type_get_by_id(ASPECT_TYPE_INT);
  else if (o1->otype->type == ASPECT_TYPE_STR)
    revm_convert_object(e1, ASPECT_TYPE_INT);
  if ((o1->otype->type != ASPECT_TYPE_INT   &&
       o1->otype->type != ASPECT_TYPE_BYTE  && 
       o1->otype->type != ASPECT_TYPE_SHORT && 
       o1->otype->type != ASPECT_TYPE_CADDR &&
       o1->otype->type != ASPECT_TYPE_DADDR &&
       o1->otype->type != ASPECT_TYPE_LONG) ||
      (o1->otype->type != o2->otype->type && revm_convert_object(e2, o1->otype->type)))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter has not INTEGER type", -1);

  /* Get the objects */
  dst  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
  src  = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));

  /* Compute */
  switch (op)
    {
    case REVM_OP_ADD:
      dst += src;
      break;
    case REVM_OP_SUB:
      dst -= src;
      break;
    case REVM_OP_MUL:
      dst *= src;
      break;
    case REVM_OP_DIV:
      if (src == 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Cannot divide by 0", -1);      
      dst /= src;
      break;
    case REVM_OP_MOD:
      if (src == 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Cannot divide by 0", -1);
      dst %= src;
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unknown requested operation", -1);
    }

  /* Store result */
  if (o1->immed)
    o1->immed_val.ent = dst;
  else
    o1->set_obj(o1->parent, dst);
  
  /* Store last-result variable */
  if (o1->otype->type == ASPECT_TYPE_HASH ||
      o1->otype->type == ASPECT_TYPE_LIST)
    goto end;
  
  last               = revm_expr_get(REVM_VAR_RESULT);
  res		     = last->value;
  res->otype         = o1->otype;
  last->type         = o1->otype;
  res->immed_val.ent = dst;
  res->immed         = 1;
  if (!world.state.revm_quiet)
    {
      snprintf(buf, sizeof(buf), " $_ = 0x" XFMT "\n\n", dst);
      revm_output(buf);
    }
  
 end:

  /* If the object was a string, translate it back */
  if (o1->otype->type != oldtype)
    revm_convert_object(e1, oldtype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* API for adding in hash */
int			revm_hash_add(hash_t *h, revmexpr_t *e)
{
  eresi_Addr		elem;
  unsigned char		*key;
  hash_t		*src;
  revmobj_t		*o;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);
  o = e->value;
  if (!o)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameter type", -1);
  
  /* If the source object is a hash, then we need to do a merge operation ! */
  if (o->otype->type == ASPECT_TYPE_HASH)
    {
      src = (hash_t *) o->get_obj(o->parent);
      hash_merge(h, src);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Some checks */
  if (!o->kname && !o->hname && !o->get_name)
    {
      if (revm_convert_object(e, ASPECT_TYPE_STR) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown key for source object", -1);
      key = (unsigned char *) (h->type != o->otype->type ? strdup(o->immed_val.str) : o->immed_val.str);
    }
  else
    key = (unsigned char *) (o->kname ? o->kname : (o->hname ? o->hname : o->get_name(o->root, o->parent)));
	   
  if (!key)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown key for source object", -1);

  /* In case the hash table was freshly created, assign its element type now */
  if (h->type == ASPECT_TYPE_UNKNOW)
    h->type = o->otype->type;

  /* Make sure we insert an element of the same type */
  if (h->type != o->otype->type && revm_convert_object(e, h->type))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Incompatible types between objects", -1);
   
  /* Add it to the hash table */
  elem = (eresi_Addr) (o->immed ? o->immed_val.ent : o->get_obj(o->parent));
  hash_add(h, (char *) key, (void *) elem);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* API for adding in hash */
int			revm_elist_add(list_t *h, revmexpr_t *e)
{
  eresi_Addr		elem;
  char			*key;
  list_t		*src;
  revmobj_t		*copy;
  revmobj_t		*o;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);
  o = e->value;
  if (!o)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameter type", -1);

  /* If the source object is a hash, then we need to do a merge operation ! */
  if (o->otype->type == ASPECT_TYPE_LIST)
    {
      src = (list_t *) o->get_obj(o->parent);
      elist_merge(h, src);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Some checks */
  if (!o->kname && !o->hname && !o->get_name)
    {
      copy = revm_copy_object(o);
      if (revm_convert_object(e, ASPECT_TYPE_STR) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown key for source object", -1);
      key = strdup(e->value->immed_val.str);
      revm_destroy_object(e->value);
      o = e->value = copy;
    }
  else
    {
      key = (o->kname ? o->kname : o->hname ? o->hname : o->get_name(o->root, o->parent));
      key = strdup(key);
    }
  if (!key)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown key for source object", -1);

  /* In case the hash table was freshly created, assign its element type now */
  if (h->type == ASPECT_TYPE_UNKNOW)
    h->type = o->otype->type;

  /* Make sure we insert an element of the same type */
  if (h->type != o->otype->type && revm_convert_object(e, h->type))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Incompatible types between objects", -1);
   
  /* Add it to the hash table */
  elem = (eresi_Addr) (o->immed ? o->immed_val.ent : o->get_obj(o->parent));
  elist_add(h, key, (void *) elem);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* API for deleting in hash */
int			revm_hash_del(hash_t *h, revmexpr_t *e)
{
  char			*name;
  hash_t		*src;
  revmobj_t		*o;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);
  o = e->value;
  if (!o)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameter type", -1);

  /* If the source object is a hash, then we need to do a merge operation ! */
  if (o->otype->type == ASPECT_TYPE_HASH)
    {
      src = (hash_t *) o->get_obj(o->parent);
      hash_unmerge(h, src);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* If second parameter was a string */
  if (o->otype->type == ASPECT_TYPE_STR)
    {
      if (o->get_name)
	name = o->get_name(o->root, o->parent);
      else
	name = (o->immed ? o->immed_val.str : (char *) o->get_obj(o->parent));
      if (hash_get(h, name))
	hash_del(h, name);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Else if it was a hash element */
  if ((h->type != o->otype->type && revm_convert_object(e, h->type)) || !o->kname ||
      !hash_get(h, o->kname))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown hash element to remove", -1);
  hash_del(h, o->kname);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* REVM API for deleting in lists */
int			revm_elist_del(list_t *h, revmexpr_t *e)
{
  char			*name;
  list_t		*src;
  revmobj_t		*o;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!h || !e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);
  o = e->value;
  if (!o)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameter type", -1);

  /* If the source object is a hash, then we need to do a merge operation ! */
  if (o->otype->type == ASPECT_TYPE_LIST)
    {
      src = (list_t *) o->get_obj(o->parent);
      elist_unmerge(h, src);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* If second parameter was a string */
  if (o->otype->type == ASPECT_TYPE_STR)
    {
      if (o->get_name)
	name = o->get_name(o->root, o->parent);
      else
	name = (o->immed ? o->immed_val.str : (char *) o->get_obj(o->parent));
      if (elist_get(h, name))
	elist_del(h, name);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Else if it was a hash element */
  if ((h->type != o->otype->type && revm_convert_object(e, h->type)) || !o->kname ||
      !elist_get(h, o->kname))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown hash element to remove", -1);
  elist_del(h, o->kname);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* API for setting elements inside hash */
int			revm_hash_set(char *table, char *elmname, void *obj, unsigned int type)
{
  hash_t		*h;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!table)
    hash_register((hash_t *) obj, elmname);
  else
    {
      h = hash_find(table);
      if (!h)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown destination hash table", -1);
      if (h->type == ASPECT_TYPE_UNKNOW)
	h->type = type;
      hash_add(h, elmname, obj);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* API for setting elements inside lists */
int			revm_elist_set(char *table, char *elmname, void *obj, unsigned int type)
{
  list_t		*h;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!table)
    elist_register((list_t *) obj, elmname);
  else
    {
      h = elist_find(table);
      if (!h)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown destination list", -1);
      if (h->type == ASPECT_TYPE_UNKNOW)
	h->type = type;
      elist_add(h, elmname, obj);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * @brief Copy a simply typed object into another one
 * @param o1 Destination object
 * @param o2 Source object
 * @return Success:0 Error:-1
 */
/* o1 = destination, o2 = source */
int			revm_object_set(revmexpr_t *e1, revmexpr_t *e2)
{
  revmobj_t		*o1;
  revmobj_t		*o2;
  char                  *str;
  eresi_Addr            val64;
  unsigned int                 val32;
  unsigned char                val8;
  u_short               val16;
  hash_t		*hash;
  list_t		*list;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  o1 = e1->value;
  o2 = e2->value;

  /* Do the real assignation */
  switch (o1->otype->type)
    {
    case ASPECT_TYPE_STR:
      str = (o2->immed ? o2->immed_val.str : o2->get_name(o2->root, o2->parent));
      if (o1->immed)
        {
          o1->immed_val.str = strdup(str);
          o1->size = o2->size;
        }
      else if (o1->hname && (o1->kname || o2->kname))
	{
	  if (o1->contype == CONT_HASH)
	    revm_hash_set(o1->hname, o1->kname ? o1->kname : o2->kname, str,
			ASPECT_TYPE_STR);
	  else if (o1->contype == CONT_LIST)
	    revm_elist_set(o1->hname, o1->kname ? o1->kname : o2->kname, str,
			ASPECT_TYPE_STR);
	}
      else if (o1->parent == str || !o1->parent)
	o1->parent = (str ? strdup(str) : NULL);
      else if (o1->set_name(o1->root, o1->parent, str) < 0)
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__, str);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to set string variable", -1);
	}
      break;

    case ASPECT_TYPE_BYTE:
      if (o1->otype->isptr)
	goto ptrcopy;
      val8 = (o2->immed ? o2->immed_val.byte : o2->get_obj(o2->parent));
      if (o1->immed)
        o1->immed_val.byte = val8;
      else if (o1->hname && (o1->kname || o2->kname))
	{
	  if (o1->contype == CONT_HASH)
	    revm_hash_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			  (void *) (eresi_Addr) val8, ASPECT_TYPE_BYTE);
	  else if (o1->contype == CONT_LIST)
	    revm_elist_set(o1->hname, o1->kname ? o1->kname : o2->kname, str,
			ASPECT_TYPE_STR);
	}
      else if (o1->set_obj(o1->parent, val8) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set byte variable", -1);
      break;

    case ASPECT_TYPE_SHORT:
      val16 = (o2->immed ? o2->immed_val.half : o2->get_obj(o2->parent));
      if (o1->immed)
	o1->immed_val.half = val16;
      else if (o1->hname && (o1->kname || o2->kname))
	{
	  if (o1->contype == CONT_HASH)
	    revm_hash_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			  (void *) (eresi_Addr) val16, ASPECT_TYPE_SHORT);
	  else if (o1->contype == CONT_LIST)
	    revm_elist_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			   (void *) (eresi_Addr) val16, ASPECT_TYPE_SHORT);
	  
	}
      else if (o1->set_obj(o1->parent, val16) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set short variable", -1);
      break;

    case ASPECT_TYPE_INT:
    intcopy:
      val32 = (o2->immed ? o2->immed_val.word : o2->get_obj(o2->parent));
      if (o1->immed)
	o1->immed_val.word = val32;
      else if (o1->hname && (o1->kname || o2->kname))
	{
	  if (o1->contype == CONT_HASH)
	    revm_hash_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			  (void *) (eresi_Addr) val32, ASPECT_TYPE_INT);
	  else if (o1->contype == CONT_LIST)
	    revm_elist_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			   (void *) (eresi_Addr) val32, ASPECT_TYPE_INT);
	  
	}
      else if (o1->set_obj(o1->parent, val32) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set integer variable", -1);
      break;

    case ASPECT_TYPE_LONG:
      if (sizeof(unsigned long) == 4)
	{
	  o1->set_obj = revm_int_setobj;
	  goto intcopy;
	}
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
    ptrcopy:
      val64 = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
      if (o1->immed)
	o1->immed_val.ent = val64;
      else if (o1->hname && (o1->kname || o2->kname))
	{
	  if (o1->contype == CONT_HASH)
	    revm_hash_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			  (void *) val64, o1->otype->type);
	  else if (o1->contype == CONT_LIST)
	    revm_elist_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			  (void *) val64, o1->otype->type);
	  
	}
      else if (o1->set_obj(o1->parent, val64) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set long variable", -1);
      break;

    case ASPECT_TYPE_HASH:
      hash = (hash_t *) o2->get_obj(o2->parent);
      if (!o1->otype->isptr)
	{
	  if (revm_hash_set(NULL, o1->hname, (void *) hash, o1->otype->type))
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Unable to set hash table variable", -1);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
      o1->immed = 1;
      goto ptrcopy;
    case ASPECT_TYPE_LIST:
      list = (list_t *) o2->get_obj(o2->parent);
      if (!o1->otype->isptr)
	{
	  if (revm_elist_set(NULL, o1->hname, (void *) list, o1->otype->type))
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Unable to set list variable", -1);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
      o1->immed = 1;
      goto ptrcopy;
    default:
      *o1 = *o2;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Comparison function */
int			revm_object_compare(revmexpr_t *e1, revmexpr_t *e2, eresi_Addr *val)
{
  revmexpr_t		*last;
  revmobj_t		*res;
  revmobj_t		*o1;
  revmobj_t		*o2;
  char			*str;
  char			*str2;
  eresi_Addr	       	val2;
  unsigned char		bval2;
  u_short		hval2;
  unsigned int			ival2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!e2 || !e2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);
  o1 = e1->value;
  o2 = e2->value;
  if (!o1 || !o2 || !o1->otype || !o2->otype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameter type", -1);

  /* Lazy typing in action */
  if ((o1->otype->type != ASPECT_TYPE_INT   && 
       o1->otype->type != ASPECT_TYPE_BYTE  && 
       o1->otype->type != ASPECT_TYPE_SHORT && 
       o1->otype->type != ASPECT_TYPE_CADDR && 
       o1->otype->type != ASPECT_TYPE_DADDR && 
       o1->otype->type != ASPECT_TYPE_LONG  &&
       o1->otype->type != ASPECT_TYPE_STR) ||
      o1->otype->type != o2->otype->type)
    {
      revm_convert_object(e2, o1->otype->type);
      if (o2->otype->type != o1->otype->type)
	{
	  if (!o2->perm && o2->immed && 
	      o2->otype->type == ASPECT_TYPE_STR && str != NULL)
	    XFREE(__FILE__, __FUNCTION__, __LINE__,str);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid parameters", -1);
	}
    }
  str = NULL;

  /* Do the real assignation */
  switch (o1->otype->type)
    {
    case ASPECT_TYPE_STR:
      str2 = (o2->immed ? o2->immed_val.str : o2->get_name(o2->root, o2->parent));
      str  = (o1->immed ? o1->immed_val.str : o1->get_name(o1->root, o1->parent));
      if (!str || !str2)
	*val = 1;
      else
	*val = strcmp(str, str2);
      break;
    case ASPECT_TYPE_BYTE:
      bval2 = (o2->immed ? o2->immed_val.byte : o2->get_obj(o2->parent));
      *val  = (o1->immed ? o1->immed_val.byte : o1->get_obj(o1->parent));
      *val = *val & 0x000000FF;
      *val -= bval2;
      break;      

    case ASPECT_TYPE_SHORT:
      hval2  = (o2->immed ? o2->immed_val.half : o2->get_obj(o2->parent));
      *val  = (o1->immed ? o1->immed_val.half : o1->get_obj(o1->parent));
      *val = *val & 0x0000FFFF;
      *val -= hval2;
      break;      

    case ASPECT_TYPE_INT:
      ival2  = (o2->immed ? o2->immed_val.word : o2->get_obj(o2->parent));
      *val  = (o1->immed ? o1->immed_val.word : o1->get_obj(o1->parent));
      *val = *val & 0xFFFFFFFF;
      *val -= ival2;
      break;      

    case ASPECT_TYPE_LONG:
      val2  = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
      *val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
      if (sizeof(unsigned long) == 4)
	{
	  val2 = val2 & 0xFFFFFFFF;
	  *val = *val & 0xFFFFFFFF;
	}
      *val -= val2;
      break;      

    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
      val2  = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
      *val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
      *val -= val2;
      break;      

    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Uncomparable parameter type", -1);
    }
  
  /* Set the last result variable */
  last = revm_expr_get(REVM_VAR_RESULT);
  if (!last)
    {
      if (!o2->perm && o2->immed && o2->otype->type == ASPECT_TYPE_STR && str != NULL)
	XFREE(__FILE__, __FUNCTION__, __LINE__,str);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to get last result variable", -1);
    }
  revm_expr_destroy(REVM_VAR_RESULT);
  res = revm_create_IMMED(ASPECT_TYPE_INT, 1, *val);
  last = revm_expr_create_from_object(res, REVM_VAR_RESULT);
  if (!last)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to set result expression", (-1));

  /* Success */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}



/* Test a bit in a bitfield */
int			revm_testbit(revmexpr_t *e1, revmexpr_t *e2, unsigned int *result)
{
  revmobj_t		*o1;
  revmobj_t		*o2;
  revmobj_t		*res;
  revmexpr_t		*last;
  eresi_Addr	       	val;
  eresi_Addr	       	val2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!e1 || !e2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);
  o1 = e1->value;
  o2 = e2->value;
  if (!o1 || !o2 || o1->otype || !o2->otype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters types", -1);

  /* Lazy typing in action */
  if ((o1->otype->type != ASPECT_TYPE_INT   && 
       o1->otype->type != ASPECT_TYPE_BYTE  && 
       o1->otype->type != ASPECT_TYPE_SHORT && 
       o1->otype->type != ASPECT_TYPE_CADDR && 
       o1->otype->type != ASPECT_TYPE_DADDR && 
       o1->otype->type != ASPECT_TYPE_LONG) ||
      o1->otype->type != o2->otype->type)
    {
      revm_convert_object(e2, o1->otype->type);
      if (o2->otype->type != o1->otype->type)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid parameters", -1);
    }

  /* Set the last result variable */
  last = revm_expr_get(REVM_VAR_RESULT);
  if (!last || !last->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get last result variable", -1);
  res = last->value;
  res->otype = aspect_type_get_by_id(ASPECT_TYPE_INT);
  last->type = res->otype;

  /* Do the real assignation */
  val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
  val2 = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
  res->immed_val.ent = *result = ((val & (1 << val2)) >> val2);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
