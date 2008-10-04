/**
 ** @file atomic.c
 ** @ingroup libstderesi
 ** @brief Implement arithmetic operations
 **
 ** Started on  Sun Feb  9 22:43:34 2003 jfv
 ** $Id: atomic.c,v 1.4 2008-02-16 12:32:27 thor Exp $
 */
#include "libstderesi.h"



/* SET command */
int			cmd_set()
{
  revmexpr_t		*e1;
  revmexpr_t		*e2;
  int                   error;
  int			errvar;
  revmexpr_t		*last;
  unsigned int			oid;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Resolve all possible case between expressions and objects */
  error  = -1;
  errvar = 0;
  e1 = revm_expr_get(world.curjob->curcmd->param[0]);
  e2 = revm_expr_get(world.curjob->curcmd->param[1]);

  /* The $_ variable is updated as well */
  last = revm_expr_get(REVM_VAR_RESULT);
  if (last == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get result variable", -2);

  /* Make sure we lookup an expression with container if given by ID */
  if (e2 && e2->value && e2->type->type == ASPECT_TYPE_OID)
    {
      oid = (e2->value->immed ? e2->value->immed_val.word : e2->value->get_obj(e2->value->parent));
      e2 = revm_expr_lookup(oid);
      if (!e2)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to get expression from id", -1);
    }

  /* Assignment between existing expressions */
  if (e1 && e2)
    {
      errvar = revm_expr_set(e1, e2);
      if (errvar < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set expressions", (-1));

      /* Dont copy the result variable over itself if we use $_ as a source variable */
      if (strcmp(e2->label, REVM_VAR_RESULT))
	{
	  revm_expr_destroy(last->label);
	  last = revm_expr_copy(e2, REVM_VAR_RESULT, 0);
	  if (!last)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Unable to set result expression", (-1));
	}

      if (!world.state.revm_quiet)
	revm_output(" [*] Expression set succesfully \n\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* Fix the source expression if unresolved */
  if (!e2)
    {
      e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
      if (!e2)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to create source object", (-1));
    }
  
  /* Fix the destination expression */
  if (!e1)
    {
      e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
      if (!e1)
	{
	  revm_expr_destroy(e2->label);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid destination variable", (-1));
	}
    }
  
  /* Set converted value */
  if (e2->value)
    {
      if (!e1->type && revm_convert_object(e1, e2->type->type) < 0)
	{
	  if (e2->value && !e2->value->perm)
	    revm_expr_destroy(e2->label);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to convert destination type expression", (-1));
	}
      if (e1->type->type != e2->type->type &&
	  revm_convert_object(e2, e1->type->type) < 0)
	{
	  if (e2->value && !e2->value->perm)
	    revm_expr_destroy(e2->label);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to convert source type expression", (-1));
	}
      if (revm_expr_set(e1, e2) < 0)
	{
	  if (e2->value && !e2->value->perm)
	    revm_expr_destroy(e2->label);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to set expression", (-1));
	}
    }

  /* Set complex type */
  else
    {
      if (!e1->type)
	{
	  revm_expr_destroy(e1->label);
	  e1 = revm_expr_copy(e2, world.curjob->curcmd->param[0], 0);
	  if (!e1)
	    {
	      if (e2->value && !e2->value->perm)
		revm_expr_destroy(e2->label);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			   "Unable to set expressions", (-1));
	    }
	}
      else if (revm_expr_set(e1, e2) < 0)
	{
	  if (e2->value && !e2->value->perm)
	    revm_expr_destroy(e2->label);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to set expression", (-1));
	}
    }

  /* Copy the result in the last result variable */
  if (e2->type->type != ASPECT_TYPE_HASH && e2->type->type != ASPECT_TYPE_LIST)
    {
      revm_expr_destroy(last->label);
      last = revm_expr_copy(e2, REVM_VAR_RESULT, 0);
      if (!last)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to set result expression", (-1));
    }

  /* Everything OK */
  if (e1->value && !e1->value->perm)
    revm_expr_destroy(e1->label);
  if (e2->value && !e2->value->perm)
    revm_expr_destroy(e2->label);

  if (!world.state.revm_quiet)
    revm_output(" [*] Expression set succesfully \n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Comparison command */
int			cmd_cmp()
{
  revmexpr_t		*e1;
  revmexpr_t		*e2;
  char			logbuf[BUFSIZ];
  int			res;
  eresi_Addr		val;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First check if we are dealing with exprs */
  e1 = revm_expr_get(world.curjob->curcmd->param[0]);
  e2 = revm_expr_get(world.curjob->curcmd->param[1]);
  if (e1 && e2)
    {
      res = revm_expr_compare(e1, e2, &val);
      if (res < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to compare expressions", (-1));
      goto end;
    }
  
  if (!e2) 
    e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e1)
    e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  if (!e1 || !e2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup parameters", -1);
  if (!e1->type || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Parameter has type unknown thus uncomparable", -1);
  
  /* Error checking */
  res = revm_expr_compare(e1, e2, &val);
  if (e1->value && !e1->value->perm)
    revm_expr_destroy(e1->label);
  if (e2->value && !e2->value->perm)
    revm_expr_destroy(e2->label);
  if (res < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Error while setting result variable", res);

  /* Result reporting */
 end:
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " [*] Objects are %s. \n\n", (!val ? "EQUALS" : "INEQUALS"));
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Bit testing command */
int			cmd_test()
{
  revmexpr_t		*e1;
  revmexpr_t		*e2;
  char			logbuf[BUFSIZ];
  unsigned int			res;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e2) 
    e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e1)
    e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  if (!e1 || !e2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup parameters", -1);
  if (!e1->type || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Parameter has type unknown thus uncomparable", -1);

  /* Perform the operation */
  if (revm_testbit(e1, e2, &res) < 0)
    {
      if (e1->value && !e1->value->perm)
	revm_expr_destroy(e1->label);
      if (e2->value && !e2->value->perm)
	revm_expr_destroy(e2->label);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Error while setting result variable", -1);
    }

  if (e1->value && !e1->value->perm)
    revm_expr_destroy(e1->label);
  if (e2->value && !e2->value->perm)
    revm_expr_destroy(e2->label);

  /* Everything was OK */
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] $_ = %u \n\n", res);
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/************************ Now arithmetic commands *****************/



/* ADD command */
int			cmd_add()
{
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
  e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e2) 
    e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e1)
    e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  if (!e1 || !e2 || !e1->value || !e2->value || !e1->type || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup parameters", -1);
  o1 = e1->value;
  o2 = e2->value;

  /* In case we have a hash table as parameter */
  if (o1->otype->type == ASPECT_TYPE_HASH)
    {
      ret = revm_hash_add(o1->parent, e2);		
      revm_expr_destroy(e1->label);		// destroy the source alias variable
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to insert hash element", -1);
      if (!world.state.revm_quiet)
	revm_output(" [*] Element inserted succesfully\n\n");
      goto end;
    }

  /* In case we have a hash table as parameter */
  else if (o1->otype->type == ASPECT_TYPE_LIST)
    {
      ret = revm_elist_add(o1->parent, e2);		
      revm_expr_destroy(e1->label);		// destroy the source alias variable
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to insert list element", -1);
      if (!world.state.revm_quiet)
	revm_output(" [*] Element inserted succesfully\n\n");
      goto end;
    }

  /* Perform computation */
  if (revm_arithmetics(e1, e2, REVM_OP_ADD) < 0)
    {
      if (e2 && e2->value && !e2->value->perm)
	revm_expr_destroy(e2->label);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to add elements", -1);
    }
  
  /* Return success */
 end:
  if (e2 && e2->value && !e2->value->perm)
    revm_expr_destroy(e2->label);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* SUB command */
int			cmd_sub()
{
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
  e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e2) 
    e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e1)
    e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  if (!e1 || !e2 || !e1->value || !e2->value || !e1->type || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup parameters", -1);
  o1 = e1->value;
  o2 = e2->value;

  /* In case we have a hash table as parameter */
  if (o1->otype->type == ASPECT_TYPE_HASH)
    {
      ret = revm_hash_del(o1->parent, e2);
      if (ret < 0)
	{
	  if (e2 && e2->value && !e2->value->perm)
	    revm_expr_destroy(e2->label);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to delete hash element", -1);
	}
      if (!world.state.revm_quiet)
	revm_output(" [*] Element deleted succesfully\n\n");
      goto end;
    }

  /* In case we have a hash table as parameter */
  else if (o1->otype->type == ASPECT_TYPE_LIST)
    {
      ret = revm_elist_del(o1->parent, e2);
      if (ret < 0)
	{
	  if (e2 && e2->value && !e2->value->perm)
	    revm_expr_destroy(e2->label);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to delete list element", -1);
	}
      if (!world.state.revm_quiet)
	revm_output(" [*] Element deleted succesfully\n\n");
      goto end;
    }
  
  /* Perform computation */
  if (revm_arithmetics(e1, e2, REVM_OP_SUB) < 0)
    {
      if (e2 && e2->value && !e2->value->perm)
	revm_expr_destroy(e2->label);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to substract elements", -1);
    }

 end:
  /* Everything was OK */
  if (e2 && e2->value && !e2->value->perm)
    revm_expr_destroy(e2->label);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* MUL command */
int			cmd_mul()
{
  revmexpr_t		*e1;
  revmexpr_t		*e2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || 
      world.curjob->curcmd->param[1] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Need 2 parameters", -1);
  e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e2) 
    e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e1)
    e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  if (!e1 || !e2 || !e1->value || !e2->value || !e1->type || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup parameters", -1);

  /* Perform computation */
  if (revm_arithmetics(e1, e2, REVM_OP_MUL) < 0)
    {
      if (e2 && e2->value && !e2->value->perm)
	revm_expr_destroy(e2->label);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to multiply elements", -1);
    }

  /* Everything was OK */
  if (e2 && e2->value && !e2->value->perm)
    revm_expr_destroy(e2->label);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* DIV command */
int			cmd_div()
{
  revmexpr_t		*e1;
  revmexpr_t		*e2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || 
      world.curjob->curcmd->param[1] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Need 2 parameters", -1);
  e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e2) 
    e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e1)
    e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  if (!e1 || !e2 || !e1->value || !e2->value || !e1->type || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup parameters", -1);

  /* Perform computation */
  if (revm_arithmetics(e1, e2, REVM_OP_DIV) < 0)
    {
      if (e2 && e2->value && !e2->value->perm)
	revm_expr_destroy(e2->label);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to divide elements", -1);
    }
  
  /* Everything was OK */
  if (e2 && e2->value && !e2->value->perm)
    revm_expr_destroy(e2->label);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* MOD command */
int			cmd_mod()
{
  revmexpr_t		*e1;
  revmexpr_t		*e2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || 
      world.curjob->curcmd->param[1] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Need 2 parameters", -1);
  e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e2) 
    e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e1)
    e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  if (!e1 || !e2 || !e1->value || !e2->value || !e1->type || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to lookup parameters", -1);

  /* Perform computation */
  if (revm_arithmetics(e1, e2, REVM_OP_MOD) < 0)
    {
      if (e2 && e2->value && !e2->value->perm)
	revm_expr_destroy(e2->label);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to divide elements", -1);
    }

  /* Everything was OK */
  if (e2 && e2->value && !e2->value->perm)
    revm_expr_destroy(e2->label);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


