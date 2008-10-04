/*
** @file foreach.c
** @ingroup libstderesi
** @brief Implement the iteration on arrays, lists, and hash tables.
**
** Started on Wed Feb 28 19:19:04 2007 jfv
**
** $Id: foreach.c,v 1.3 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"


/* Start an iteration */
int		cmd_foreach()
{
  revmexpr_t	*indexpr;
  revmobj_t	*var;
  unsigned char	flag;
  char		**keys;
  int		keynbr;
  hash_t	*table;
  list_t	*list;
  int		index;
  void		*elem;
  int		infbound;
  int		upbound;
  unsigned int		typeid;
  eresi_Addr	lastvalue;
  char		*setname;
  revmexpr_t	*setexpr;
  revmobj_t	*setobj;
  char		*typename;
  char		*indname;
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  table = NULL;
  list = NULL;
  addr = 0;

  /* Depends the mode we are in */
  flag = (world.curjob->curcmd->argc == 3   ? 1 : 
	  world.curjob->curcmd->use_regx[0] ? 2 : 3);

  /* Create or get the induction variable */
  indexpr = revm_lookup_var(world.curjob->curcmd->param[0]);
  if (!indexpr)
    {
      if (world.curjob->curcmd->listidx != REVM_IDX_UNINIT)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to find induction variable", -1);
      var = revm_create_IMMED(ASPECT_TYPE_UNKNOW, 1, 0);
      indexpr = revm_expr_create_from_object(var, world.curjob->curcmd->param[0]);
    }
  else
    var = indexpr->value;

  /* Perform subcommand */
  switch (flag)
    {
      /* foreach elmvar of list/hash [as regx] */
    case 1:
    case 2:

      /* First try to lookup the container name */
      setexpr = revm_lookup_param(world.curjob->curcmd->param[2]);
      setobj = (setexpr && setexpr->type ? setexpr->value : NULL);
      setname = NULL;
      if (setobj)
	{
	  if (setobj->otype->type == ASPECT_TYPE_STR)
	    setname = (setobj->immed ? setobj->immed_val.str : 
			 setobj->get_name(setobj->root, setobj->parent));
	  else if (setobj->otype->type == ASPECT_TYPE_HASH ||
		   setobj->otype->type == ASPECT_TYPE_LIST)
	    {
	      setname = (setobj->kname ? setobj->kname : setobj->hname);
	      if (!setname && setobj->otype->isptr)
		{
		  if (setobj->otype->type == ASPECT_TYPE_HASH)
		    {
		      table = (hash_t *) (setobj->immed ? setobj->immed_val.ent : 
					  setobj->get_obj(setobj->parent));
		      if (!table)
			PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				     "Unable to find table name", -1);
		      setname = table->name;
		    }
		  else if (setobj->otype->type == ASPECT_TYPE_LIST)
		    {
		      list = (list_t *) (setobj->immed ? setobj->immed_val.ent : 
					 setobj->get_obj(setobj->parent));
		      if (!list)
			PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				     "Unable to find list name", -1);
		      setname = list->name;
		    }
		}
	    }
	}
      else
	setname = world.curjob->curcmd->param[2];
      if (!setname)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to find iterator name", -1);
      setname = strdup(setname);
      if (setexpr)
	revm_expr_destroy(setexpr->label);
      
      //fprintf(stderr, "Found setname = %s in foreach ! \n", setname);

      /* Try to find a hash or a list out of this variable, if not already found */
      if (!table && !list)
	{
	  table = hash_find(setname);
	  if (!table)
	    {
	      list = elist_find(setname);
	      if (!list)
		{
		  XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
		  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			       "Unable to find hash table", -1);
		}
	    }
	}

      /* See if the list or hash is already being iterated */
      if (list)
	{
	  if (world.curjob->curcmd->listidx == REVM_IDX_UNINIT && elist_linearity_get(list))
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Cannot iterate again on linearly typed list", -1);
	    }
	  elist_linearity_set(list, 1);
	  keys = elist_get_keys(list, &keynbr);
	  world.curjob->iter.list = list;
	}
      else
	{
	  if (world.curjob->curcmd->listidx == REVM_IDX_UNINIT && hash_linearity_get(table))
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Cannot iterate again on linearly typed table", -1);
	    }
	  hash_linearity_set(table, 1);
	  keys = hash_get_keys(table, &keynbr);
	}

      /* Use the correct keys array index depending on foreach iteration nbr */
    nextelem:
      if (world.curjob->curcmd->listidx == REVM_IDX_UNINIT)
	{
	  index = 0;
	  world.curjob->curcmd->listidx += 2;
	}
      
      /* The induction variable existed already : record its previous value into the parent list */
      else
	{
	  index = world.curjob->curcmd->listidx++;
	  if (flag != 2 || !regexec(&world.curjob->curcmd->regx[0], keys[index - 1], 0, 0, 0))
	    {
	      
	      /* No need to back-update if the data type is complex */
	      if (indexpr->type && aspect_type_simple(indexpr->type->type) && !indexpr->type->next)
		{
		  lastvalue = (var->immed ? var->immed_val.ent : var->get_obj(var->parent));
		  if (table)
		    hash_set(table, keys[index - 1], (void *) lastvalue);
		  else
		    elist_set(list, keys[index - 1], (void *) lastvalue);
		  //printf("back-assignment into the list for element %s at index %u of value %X \n", 
		  //keys[index - 1], index - 1, lastvalue);
		}
	    }
	}

      /* Export iteration information to revmjob so it can be used in other commands */
      if (!table && list)
	{
	  world.curjob->iter.curindex = &world.curjob->curcmd->listidx;
	  world.curjob->iter.curkey   = keys[index];
	  world.curjob->iter.curname  = world.curjob->curcmd->param[0];
	}

      /* Bound check : go to the loop end when necessary */
      if (index >= keynbr)
	{
	  if (!world.curjob->curcmd->endlabel)
	    cmd_quit();
	  if (table)
	    hash_linearity_set(table, 0);
	  else if (list)
	    elist_linearity_set(list, 0);
	  world.curjob->curcmd->listidx = REVM_IDX_UNINIT;
	  revm_move_pc(world.curjob->curcmd->endlabel);
	  XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}

      /* Get the next elem if the current one is not matching */
      if (flag == 2 && regexec(&world.curjob->curcmd->regx[0], 
			       keys[index], 0, 0, 0))
	goto nextelem;

      /* The element matched, process it */
      if (table)
	{
	  elem = hash_get(table, keys[index]);
	  typeid = table->type;
	}
      else
	{
	  elem = elist_get(list, keys[index]);
	  typeid = list->type;
	}

      /* If the element is already an expression, copy it to the induction variable */
      if (typeid == ASPECT_TYPE_EXPR)
	{
	  indname = strdup(indexpr->label);
	  revm_expr_destroy(indexpr->label);
	  indexpr = revm_expr_copy((revmexpr_t *) elem, indname, 0);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}

      /* If the element is not an expression, act depending on its type */
      if (!indexpr->type || indexpr->type->type != typeid)
        revm_convert_object(indexpr, typeid);
      if (indexpr->type->type != typeid)
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid type for induction variable", -1);
	}

      /* Update depending if the type is simple or complex */
      if (aspect_type_simple(indexpr->type->type) && !indexpr->type->next)
	{
	  var->immed = 1;
	  var->immed_val.ent = (eresi_Addr) elem;
	  //fprintf(stderr, "Setting indvar->immed_val.ent = %X \n", (eresi_Addr) elem);
	}
      else 
	{
	  typename = indexpr->type->name;
	  revm_expr_destroy(world.curjob->curcmd->param[0]);
	  indexpr = revm_inform_type_addr(typename, world.curjob->curcmd->param[0], 
					  (eresi_Addr) elem, NULL, 0, 1);
	  if (!indexpr)
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Unable to create expression for induction variable", -1);
	    }
	}
      break;
      
      /* foreach idxvar of minint until maxint */
    case 3:
      if (world.curjob->curcmd->listidx == REVM_IDX_UNINIT)
	{
	  infbound = revm_lookup_index(world.curjob->curcmd->param[2]);
	  world.curjob->curcmd->listidx = infbound + 1;
	}
      else
	infbound = world.curjob->curcmd->listidx++;

      upbound  = revm_lookup_index(world.curjob->curcmd->param[4]);

      /* Bound check */
      if (infbound >= upbound)
	{
	  if (!world.curjob->curcmd->endlabel)
	    cmd_quit();
	  world.curjob->curcmd->listidx = REVM_IDX_UNINIT;
	  revm_move_pc(world.curjob->curcmd->endlabel);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}

      /* Set the induction variable */
      if (!indexpr->type || indexpr->type->type != ASPECT_TYPE_INT)
        revm_convert_object(indexpr, ASPECT_TYPE_INT);
      if (!indexpr->type || indexpr->type->type != ASPECT_TYPE_INT)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid type for induction variable", -1);

      var->immed = 1;
      var->immed_val.word = infbound++;
      break;
    }
  
  /* Support when until goes decreasing */
  /* Here set the induction variable (3 cases : first, iterating, last) */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


 /* End an iteration */
 int		cmd_forend()
 {
   PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
   revm_move_pc(world.curjob->curcmd->endlabel);
   PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
