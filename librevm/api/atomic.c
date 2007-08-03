/*
** @file arith.c
** 
** Started on  Sun Feb  9 22:43:34 2003 jfv
**
** $Id: atomic.c,v 1.9 2007-08-03 11:51:00 heroine Exp $
**
*/
#include "revm.h"


/* Preconditions on atomic operations set */
/* Only called by set */
int                     revm_preconds_atomics(revmobj_t **o1, revmobj_t **o2)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  *o1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  *o2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!*o1 || !*o2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to lookup a parameter", -1);

  /* Lazy typing in action */
  if ((*o1)->type != (*o2)->type)
    {
      if ((*o2)->type == ASPECT_TYPE_UNKNOW)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Source parameter undefined", -1);
      if ((*o1)->type == ASPECT_TYPE_UNKNOW)
        revm_convert_object(*o1, (*o2)->type);
      else if (revm_convert_object(*o2, (*o1)->type) < 0)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "SET parameters type are not compatible", -1);
    }
  
  /* Make sure we dont want to write in a constant */
  if ((*o1)->immed && !(*o1)->perm)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Destination parameter must not be a constant", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Preconditions on arithmetic operations */
/* Used by add, sub, mul, div, mod */
int			revm_arithmetics(revmobj_t *o1, revmobj_t *o2, u_char op)
{
  elfsh_Addr		src;
  elfsh_Addr		dst;
  char			buf[BUFSIZ];
  revmobj_t		*res;
  u_char		wastr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* First basic check */
  if (!o1 || !o2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup a parameter", -1);

  /* First check if we write in a constant */
  if (o1->immed && !o1->perm)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Destination parameter must not be a constant", -1);

  /* Preliminary checks */
  wastr = 0;
  if (o1->type == ASPECT_TYPE_UNKNOW && o1->perm)
    o1->type = ASPECT_TYPE_INT;
  else if (o1->type == ASPECT_TYPE_STR)
    {
      revm_convert_object(o1, ASPECT_TYPE_INT);
      wastr = 1;
    }
  if ((o1->type != ASPECT_TYPE_INT   &&
       o1->type != ASPECT_TYPE_BYTE  && 
       o1->type != ASPECT_TYPE_SHORT && 
       o1->type != ASPECT_TYPE_CADDR &&
       o1->type != ASPECT_TYPE_DADDR &&
       o1->type != ASPECT_TYPE_LONG) ||
      (o1->type != o2->type && revm_convert_object(o2, o1->type)))
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

  res = hash_get(&vars_hash, REVM_VAR_RESULT);
  res->immed_val.ent = dst;
  res->immed = 1;
  if (!world.state.revm_quiet)
    {
      snprintf(buf, sizeof(buf), " $_ = " DFMT "\n\n", dst);
      revm_output(buf);
    }
  
  /* If the object was a string, translate it back */
  if (wastr)
    revm_convert_object(o1, ASPECT_TYPE_STR);

  if (!o2->perm)
    XFREE(__FILE__, __FUNCTION__, __LINE__, o2);
  if (!o1->perm)
    XFREE(__FILE__, __FUNCTION__, __LINE__, o1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* API for adding in hash */
int			revm_hash_add(hash_t *h, revmobj_t *o)
{
  elfsh_Addr		elem;
  u_char		*key;
  hash_t		*src;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If the source object is a hash, then we need to do a merge operation ! */
  if (o->type == ASPECT_TYPE_HASH)
    {
      src = (hash_t *) o->get_obj(o->parent);
      hash_merge(h, src);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Some checks */
  if (!o->kname && !o->hname && !o->get_name)
    {
      if (revm_convert_object(o, ASPECT_TYPE_STR) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown key for source object", -1);
      key = (u_char *) (h->type != o->type ? strdup(o->immed_val.str) : o->immed_val.str);
    }
  else
    key = (u_char *) (o->kname ? o->kname : (o->hname ? o->hname : o->get_name(o->root, o->parent)));
	   
  if (!key)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown key for source object", -1);

  /* In case the hash table was freshly created, assign its element type now */
  if (h->type == ASPECT_TYPE_UNKNOW)
    h->type = o->type;

  /* Make sure we insert an element of the same type */
  if (h->type != o->type && revm_convert_object(o, h->type))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Incompatible types between objects", -1);
   
  /* Add it to the hash table */
  elem = (elfsh_Addr) (o->immed ? o->immed_val.ent : o->get_obj(o->parent));
  hash_add(h, (char *) key, (void *) elem);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* API for adding in hash */
int			revm_list_add(list_t *h, revmobj_t *o)
{
  elfsh_Addr		elem;
  char			*key;
  list_t		*src;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If the source object is a hash, then we need to do a merge operation ! */
  if (o->type == ASPECT_TYPE_LIST)
    {
      src = (list_t *) o->get_obj(o->parent);
      list_merge(h, src);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Some checks */
  if (!o->kname && !o->hname && !o->get_name)
    {
      if (revm_convert_object(o, ASPECT_TYPE_STR) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown key for source object", -1);
      key = (h->type != o->type ? strdup(o->immed_val.str) : o->immed_val.str);
    }
  else
    key = (o->kname ? o->kname : o->hname ? o->hname : o->get_name(o->root, o->parent));
  if (!key)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown key for source object", -1);

  /* In case the hash table was freshly created, assign its element type now */
  if (h->type == ASPECT_TYPE_UNKNOW)
    h->type = o->type;

  /* Make sure we insert an element of the same type */
  if (h->type != o->type && revm_convert_object(o, h->type))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Incompatible types between objects", -1);
   
  /* Add it to the hash table */
  elem = (elfsh_Addr) (o->immed ? o->immed_val.ent : o->get_obj(o->parent));
  list_add(h, key, (void *) elem);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* API for deleting in hash */
int			revm_hash_del(hash_t *h, revmobj_t *o)
{
  char			*name;
  hash_t		*src;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If the source object is a hash, then we need to do a merge operation ! */
  if (o->type == ASPECT_TYPE_HASH)
    {
      src = (hash_t *) o->get_obj(o->parent);
      hash_unmerge(h, src);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* If second parameter was a string */
  if (o->type == ASPECT_TYPE_STR)
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
  if ((h->type != o->type && revm_convert_object(o, h->type)) || !o->kname ||
      !hash_get(h, o->kname))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown hash element to remove", -1);
  hash_del(h, o->kname);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* API for deleting in hash */
int			revm_list_del(list_t *h, revmobj_t *o)
{
  char			*name;
  list_t		*src;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If the source object is a hash, then we need to do a merge operation ! */
  if (o->type == ASPECT_TYPE_LIST)
    {
      src = (list_t *) o->get_obj(o->parent);
      list_unmerge(h, src);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* If second parameter was a string */
  if (o->type == ASPECT_TYPE_STR)
    {
      if (o->get_name)
	name = o->get_name(o->root, o->parent);
      else
	name = (o->immed ? o->immed_val.str : (char *) o->get_obj(o->parent));
      if (list_get(h, name))
	list_del(h, name);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Else if it was a hash element */
  if ((h->type != o->type && revm_convert_object(o, h->type)) || !o->kname ||
      !list_get(h, o->kname))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown hash element to remove", -1);
  list_del(h, o->kname);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* API for setting elements inside hash */
int			revm_hash_set(char *table, char *elmname, void *obj, u_int type)
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




/* API for setting elements inside hash */
int			revm_list_set(char *table, char *elmname, void *obj, u_int type)
{
  list_t		*h;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!table)
    list_register((list_t *) obj, elmname);
  else
    {
      h = list_find(table);
      if (!h)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown destination list", -1);
      if (h->type == ASPECT_TYPE_UNKNOW)
	h->type = type;
      list_add(h, elmname, obj);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* o1 = destination, o2 = source */
int			revm_object_set(revmobj_t *o1, revmobj_t *o2)
{
  revmobj_t		*last;
  char                  *str;
  elfsh_Addr            val64;
  u_int                 val32;
  u_char                val8;
  u_short               val16;
  hash_t		*hash;
  list_t		*list;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* The $_ variable is updated as well */
  last = hash_get(&vars_hash, REVM_VAR_RESULT);
  if (last == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get result variable", -2);
  last->type = o1->type;

  /* Do the real assignation */
  switch (o1->type)
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
	    revm_list_set(o1->hname, o1->kname ? o1->kname : o2->kname, str,
			ASPECT_TYPE_STR);
	}
      else if (o1->set_name(o1->root, o1->parent, str) < 0)
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__, str);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to set string variable", -1);
	}
      break;

    case ASPECT_TYPE_BYTE:
      val8 = (o2->immed ? o2->immed_val.byte : o2->get_obj(o2->parent));
      if (o1->immed)
        o1->immed_val.byte = val8;
      else if (o1->hname && (o1->kname || o2->kname))
	{
	  if (o1->contype == CONT_HASH)
	    revm_hash_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			(void *) (elfsh_Addr) val8, ASPECT_TYPE_BYTE);
	  else if (o1->contype == CONT_LIST)
	    revm_list_set(o1->hname, o1->kname ? o1->kname : o2->kname, str,
			ASPECT_TYPE_STR);
	}
      else if (o1->set_obj(o1->parent, val8) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set byte variable", -1);
      last->immed_val.byte = val8;
      break;

    case ASPECT_TYPE_SHORT:
      val16 = (o2->immed ? o2->immed_val.half : o2->get_obj(o2->parent));
      if (o1->immed)
	o1->immed_val.half = val16;
      else if (o1->hname && (o1->kname || o2->kname))
	{
	  if (o1->contype == CONT_HASH)
	    revm_hash_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			(void *) (elfsh_Addr) val16, ASPECT_TYPE_SHORT);
	  else if (o1->contype == CONT_LIST)
	    revm_list_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			(void *) (elfsh_Addr) val16, ASPECT_TYPE_SHORT);
	    
	}
      else if (o1->set_obj(o1->parent, val16) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set short variable", -1);
      last->immed_val.half = val16;
      break;

    case ASPECT_TYPE_INT:
      val32 = (o2->immed ? o2->immed_val.word : o2->get_obj(o2->parent));
      if (o1->immed)
	o1->immed_val.word = val32;
      else if (o1->hname && (o1->kname || o2->kname))
	{
	  if (o1->contype == CONT_HASH)
	    revm_hash_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			(void *) (elfsh_Addr) val32, ASPECT_TYPE_INT);
	  else if (o1->contype == CONT_LIST)
	    revm_list_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			(void *) (elfsh_Addr) val32, ASPECT_TYPE_INT);

	}
      else if (o1->set_obj(o1->parent, val32) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set integer variable", -1);
      last->immed_val.word = val32;
      break;

    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
    case ASPECT_TYPE_LONG:
      val64 = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
      if (o1->immed)
	o1->immed_val.ent = val64;
      else if (o1->hname && (o1->kname || o2->kname))
	{
	  if (o1->contype == CONT_HASH)
	    revm_hash_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			(void *) val64, o1->type);
	  else if (o1->contype == CONT_LIST)
	    revm_list_set(o1->hname, o1->kname ? o1->kname : o2->kname, 
			(void *) val64, o1->type);
	  
	}
      else if (o1->set_obj(o1->parent, val64) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set long variable", -1);
      last->immed_val.ent = val64;
      break;

    case ASPECT_TYPE_HASH:
      hash = (hash_t *) o2->get_obj(o2->parent);
      if (revm_hash_set(NULL, o1->hname, (void *) hash, o1->type))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set hash table variable", -1);
      break;

    case ASPECT_TYPE_LIST:
      list = (list_t *) o2->get_obj(o2->parent);
      if (revm_list_set(NULL, o1->hname, (void *) list, o1->type))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set list variable", -1);
      break;

    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown type for SET parameter", -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Comparison function */
int			revm_object_compare(revmobj_t *o1, revmobj_t *o2, elfsh_Addr *val)
{
  revmobj_t		*last;
  char			*str;
  char			*str2;
  elfsh_Addr	       	val2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Lazy typing in action */
  if ((o1->type != ASPECT_TYPE_INT   && 
       o1->type != ASPECT_TYPE_BYTE  && 
       o1->type != ASPECT_TYPE_SHORT && 
       o1->type != ASPECT_TYPE_CADDR && 
       o1->type != ASPECT_TYPE_DADDR && 
       o1->type != ASPECT_TYPE_LONG  &&
       o1->type != ASPECT_TYPE_STR) ||
      o1->type != o2->type)
    {
      revm_convert_object(o2, o1->type);
      if (o2->type != o1->type)
	{
	  if (!o2->perm && o2->immed && 
	      o2->type == ASPECT_TYPE_STR && str != NULL)
	    XFREE(__FILE__, __FUNCTION__, __LINE__,str);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid parameters", -1);
	}
    }
  str = NULL;

  /* Set the last result variable */
  last = hash_get(&vars_hash, REVM_VAR_RESULT);
  if (last == NULL)
    {
      if (!o2->perm && o2->immed && 
	  o2->type == ASPECT_TYPE_STR && str != NULL)
	XFREE(__FILE__, __FUNCTION__, __LINE__,str);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to get last result variable", -1);
    }
  last->type = ASPECT_TYPE_INT;

  /* Do the real assignation */
  switch (o1->type)
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
    case ASPECT_TYPE_SHORT:
    case ASPECT_TYPE_INT:
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
    case ASPECT_TYPE_LONG:
      val2  = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
      *val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
      *val -= val2;
      break;      
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Uncomparable parameter type", -1);
    }

  last->immed_val.ent = *val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}



/* Test a bit in a bitfield */
int			revm_testbit(revmobj_t *o1, revmobj_t *o2, u_int *result)
{
  revmobj_t		*last;
  elfsh_Addr	       	val;
  elfsh_Addr	       	val2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Lazy typing in action */
  if ((o1->type != ASPECT_TYPE_INT   && 
       o1->type != ASPECT_TYPE_BYTE  && 
       o1->type != ASPECT_TYPE_SHORT && 
       o1->type != ASPECT_TYPE_CADDR && 
       o1->type != ASPECT_TYPE_DADDR && 
       o1->type != ASPECT_TYPE_LONG) ||
      o1->type != o2->type)
    {
      revm_convert_object(o2, o1->type);
      if (o2->type != o1->type)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid parameters", -1);
    }

  /* Set the last result variable */
  last = hash_get(&vars_hash, REVM_VAR_RESULT);
  if (last == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get last result variable", -1);
  last->type = ASPECT_TYPE_INT;

  /* Do the real assignation */
  val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
  val2 = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
  last->immed_val.ent = *result = (val & (~val2));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
