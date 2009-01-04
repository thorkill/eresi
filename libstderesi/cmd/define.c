/*
** @file define.c
** @ingroup libstderesi.
** @brief Allow constant values to be defined on names.
**
** Start on Fri Aug 3 03:55:45 2007 jfv
**
** $Id: define.c,v 1.1 2007-11-29 14:01:56 may Exp $
*/
#include "libstderesi.h"



/* Define a new value as constant */
int			cmd_define()
{
  revmexpr_t		*orig;
  revmexpr_t		*obj;
  revmconst_t		*cons;
  int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (world.curjob->curcmd->argc < 2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);
  orig = revm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!orig || !orig->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid input parameter", -1);
  orig->value->perm = 1;

  /* Create a new constant entry */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, cons, sizeof(revmconst_t), -1);
  cons->desc = "Automatically generated constant define";
  cons->name = world.curjob->curcmd->param[0];

  /* Add many values to create a define */
  if (world.curjob->curcmd->argc != 2)
    for (index = 2; index < world.curjob->curcmd->argc; index++)
      {
	obj = revm_lookup_param(world.curjob->curcmd->param[index], 1);
	if (!obj)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid input parameters", -1);
	if (revm_arithmetics(NULL, orig, obj, REVM_OP_ADD) < 0)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to add values to be defined", -1);
	revm_expr_destroy_by_name(obj->label);
      }
  
  /* Finally put the value up */
  cons->val = orig->value->immed_val.ent;
  revm_expr_destroy_by_name(orig->label);
  hash_add(&const_hash, world.curjob->curcmd->param[0], cons);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Undefine a constant value */
int			cmd_undef()
{
  revmconst_t		*cons;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  cons = hash_get(&const_hash, world.curjob->curcmd->param[0]);
  if (!cons)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unknown defined value", -1);
  XFREE(__FILE__, __FUNCTION__, __LINE__, cons);
  hash_del(&const_hash, world.curjob->curcmd->param[0]);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
