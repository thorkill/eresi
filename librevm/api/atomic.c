/*
** arith.c for elfsh
** 
** Started on  Sun Feb  9 22:43:34 2003 mayhem
**
** $Id: atomic.c,v 1.2 2007-03-07 16:45:35 thor Exp $
**
*/
#include "revm.h"


/* Preconditions on atomic operations set */
/* Only called by set */
int                     vm_preconds_atomics(revmobj_t **o1, revmobj_t **o2)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  *o1 = vm_lookup_param(world.curjob->curcmd->param[0]);
  *o2 = vm_lookup_param(world.curjob->curcmd->param[1]);
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
        vm_convert_object(*o1, (*o2)->type);
      else if (vm_convert_object(*o2, (*o1)->type) < 0)
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
int			vm_arithmetics(revmobj_t *o1, revmobj_t *o2, u_char op)
{
  elfsh_Addr		src;
  elfsh_Addr		dst;

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
  if (o1->type == ASPECT_TYPE_UNKNOW && o1->perm)
    o1->type = ASPECT_TYPE_INT;
  else if (o1->type == ASPECT_TYPE_STR)
    vm_convert_object(o1, ASPECT_TYPE_INT);
  if ((o1->type != ASPECT_TYPE_INT   &&
       o1->type != ASPECT_TYPE_BYTE  && 
       o1->type != ASPECT_TYPE_SHORT && 
       o1->type != ASPECT_TYPE_CADDR &&
       o1->type != ASPECT_TYPE_DADDR &&
       o1->type != ASPECT_TYPE_LONG) ||
      (o1->type != o2->type && vm_convert_object(o2, o1->type)))
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
  if (!world.state.vm_quiet)
    vm_output(" [*] Field modified succesfully\n\n");
  if (!o2->perm)
    XFREE(__FILE__, __FUNCTION__, __LINE__, o2);
  if (!o1->perm)
    XFREE(__FILE__, __FUNCTION__, __LINE__, o1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* API for adding in hash */
int			vm_hash_add(hash_t *h, revmobj_t *o)
{
  elfsh_Addr		elem;
  char			*key;
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
      if (vm_convert_object(o, ASPECT_TYPE_STR) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown key for source object", -1);
      key = (h->type != o->type ? strdup(o->immed_val.str) : o->immed_val.str);
    }
  else
    key = (o->kname ? o->kname : o->hname ? o->hname : 
	   o->get_name(o->root, o->parent));
  if (!key)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown key for source object", -1);

  /* In case the hash table was freshly created, assign its element type now */
  if (h->type == ASPECT_TYPE_UNKNOW)
    h->type = o->type;

  /* Make sure we insert an element of the same type */
  if (h->type != o->type && vm_convert_object(o, h->type))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Incompatible types between objects", -1);
   
  /* Add it to the hash table */
  elem = (elfsh_Addr) (o->immed ? o->immed_val.ent : o->get_obj(o->parent));
  hash_add(h, key, (void *) elem);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* API for deleting in hash */
int			vm_hash_del(hash_t *h, revmobj_t *o)
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
  if ((h->type != o->type && vm_convert_object(o, h->type)) || !o->kname ||
      !hash_get(h, o->kname))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown hash element to remove", -1);
  hash_del(h, o->kname);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* API for setting elements inside hash */
int			vm_hash_set(char   *table, 
				    char   *elmname, 
				    void   *obj,
				    u_char type)
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
