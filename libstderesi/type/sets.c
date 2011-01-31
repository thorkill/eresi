/*
* @file libstderesi/type/sets.c
** @ingroup cmd
** @brief Primitives for manipulating sets
**
** Started Dec 29 2010 18:57:03 jfv
** $Id$
*/
#include "libstderesi.h"

/** Compute union of sets */
int			cmd_union()
{
  hash_t		*src;
  hash_t		*dst;
  revmexpr_t		*e1;
  revmexpr_t		*e2;
  revmobj_t		*o1;
  revmobj_t		*o2;
  int			ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || 
      world.curjob->curcmd->param[1] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Need 2 parameters", -1);
  e1 = revm_lookup_param(world.curjob->curcmd->param[0], 1);
  e2 = revm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!e1 || !e2 || !e1->value || !e2->value || !e1->type || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup parameters", -1);
  o1 = e1->value;
  o2 = e2->value;

  /* In case we have a hash table as parameter */
  if (o1->otype->type != ASPECT_TYPE_HASH || 
      o2->otype->type != ASPECT_TYPE_HASH)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Parameters must be set/hash/tables objects", -1);
  
  dst = (hash_t *) o1->parent;
  src = (hash_t *) o2->parent;
  
  ret = hash_merge(dst, src);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to compute union", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** Compute intesection of sets */
int			cmd_inter()
{
  hash_t		*src;
  hash_t		*dst;
  revmexpr_t		*e1;
  revmexpr_t		*e2;
  revmobj_t		*o1;
  revmobj_t		*o2;
  int			ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || 
      world.curjob->curcmd->param[1] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Need 2 parameters", -1);
  e1 = revm_lookup_param(world.curjob->curcmd->param[0], 1);
  e2 = revm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!e1 || !e2 || !e1->value || !e2->value || !e1->type || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup parameters", -1);
  o1 = e1->value;
  o2 = e2->value;

  /* In case we have a hash table as parameter */
  if (o1->otype->type != ASPECT_TYPE_HASH || 
      o2->otype->type != ASPECT_TYPE_HASH)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Parameters must be set/hash/tables objects", -1);
  
  dst = (hash_t *) o1->parent;
  src = (hash_t *) o2->parent;
  
  ret = hash_inter(dst, src);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to compute intersection", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
