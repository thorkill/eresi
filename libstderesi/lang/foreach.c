/**
 * @file libstderesi/lang/foreach.c
 * @ingroup lang
 * @brief Implement the iteration on arrays, lists, and hash tables.
 *
 * Started on Wed Feb 28 19:19:04 2007 jfv
 * $Id$
 */
#include "libstderesi.h"


/** Effectively create a new induction variable */
static revmexpr_t	*revm_induction_load(char *name)
{
  revmexpr_t		*induction;
  revmobj_t		*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We are starting a new foreach, induction variable cannot be initialized */
  assert(world.curjob->iter[world.curjob->curloop].listidx == REVM_IDX_UNINIT);

  var = revm_create_IMMED(ASPECT_TYPE_UNKNOW, 1, 0);
  induction = revm_expr_create_from_object(var, name, 1);
  world.curjob->iter[world.curjob->curloop].curind   = induction;

  world.curjob->iter[world.curjob->curloop].reclevel = world.curjob->curscope;
  world.curjob->iter[world.curjob->curloop].end      = world.curjob->curcmd->endlabel;

#if __DEBUG_FOREACH__
  fprintf(stderr, "INDUCTION_LOAD: curcmd->end = %s and iter[cur].end = %s \n",
	  world.curjob->curcmd->endlabel, world.curjob->iter[world.curjob->curloop].end);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, induction);
}



/** Top-evel for getting the induction variable from its name */
static revmexpr_t	*revm_induction_get(char *name)
{
  revmexpr_t		*induction;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If the induction variable name has changed, we entered a new foreach construct */
  if (!world.curjob->iter[world.curjob->curloop].curind)
    induction = revm_induction_load(name);

  /* If we are reusing the same name for the induction variable */
  else if (!strcmp(name, world.curjob->iter[world.curjob->curloop].curind->label))
    {
      if (world.curjob->curloop + 1 == REVM_MAXSRCNEST)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Maximum foreach depth reached, increase REVM_MAXSRCNEST value !", NULL);
      if (NULL != world.curjob->iter[world.curjob->curloop].end && 
	  NULL != world.curjob->curcmd->endlabel && 
	  strcmp(world.curjob->iter[world.curjob->curloop].end, world.curjob->curcmd->endlabel))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "An existing induction variable already has this name", NULL);
      
      /* We are executing a deeper instance of the same foreach structure */
      if (world.curjob->iter[world.curjob->curloop].reclevel != world.curjob->curscope)
	{
	  world.curjob->curloop++;
	  induction = revm_induction_load(name);
	}
      else
	induction = world.curjob->iter[world.curjob->curloop].curind;
    }

  /* If we are executing another foreach command in a current or deeper context */
  else if (world.curjob->iter[world.curjob->curloop].reclevel != world.curjob->curscope ||
	   world.curjob->iter[world.curjob->curloop].end != world.curjob->curcmd->endlabel)
    {
      world.curjob->curloop++;
      induction = revm_induction_load(name);
    }
  else
    {
      revm_expr_print(world.curjob->iter[world.curjob->curloop].curind, 0);

#if __DEBUG_FOREACH__
      fprintf(stderr, "name = %s curind->label = %s reclevel = %u curscope = %u "
	      "end = %s endlabel = %s \n",
	      name, world.curjob->iter[world.curjob->curloop].curind->label,
	      world.curjob->iter[world.curjob->curloop].reclevel,
	      world.curjob->curscope, world.curjob->iter[world.curjob->curloop].end,
	      world.curjob->curcmd->endlabel);
#endif

      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Inconsistent foreach induction variable", NULL);	   
    }

#if __DEBUG_FOREACH__
  fprintf(stderr, "INDUCTION_GET: Induction variable name = %s listidx = %u \n", name,
	  world.curjob->iter[world.curjob->curloop].listidx);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, induction);
}



/** Record the induction variable if we have changed its state since the last iteration */
static void	revm_induction_record(char flag, revmexpr_t *induction, char *curkey, 
				      hash_t *table, list_t *list)
{
  eresi_Addr	lastvalue;
  revmobj_t	*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  var = induction->value;
  if (flag != 2 || !regexec(&world.curjob->curcmd->regx[0], curkey, 0, 0, 0))
    {
      
      /* No need to back-update if the data type is complex */
      if (induction->type && aspect_type_simple(induction->type->type) && 
	  !induction->type->next)
	{
	  lastvalue = (var->immed ? var->immed_val.ent : var->get_obj(var->parent));
	  if (table)
	    hash_set(table, curkey, (void *) lastvalue);
	  else
	    elist_set(list, curkey, (void *) lastvalue);

#if __DEBUG_FOREACH__
	  fprintf(stderr, "INDUCTION_RECORD: back-assignment in list for elem %s of value %X \n", 
		  curkey, lastvalue);
#endif

	}
    }
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);  
}


/** Iterate on an array */
static int	revm_loop_array(revmexpr_t *induction, char *infstr, char *supstr)
{
  int		infbound;
  int		upbound;
  revmobj_t	*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Do we have to open a new iteration scope ? */
  if (world.curjob->iter[world.curjob->curloop].reclevel != world.curjob->curscope ||
      world.curjob->iter[world.curjob->curloop].end != world.curjob->curcmd->endlabel)
    world.curjob->curloop++;

  /* Find lower and upper bounds for this iteration */
  var = induction->value;
  if (world.curjob->iter[world.curjob->curloop].listidx == REVM_IDX_UNINIT)
    {
      infbound = revm_lookup_index(infstr);
      world.curjob->iter[world.curjob->curloop].listidx = infbound + 1;
    }
  else
    infbound = world.curjob->iter[world.curjob->curloop].listidx++;
  
  upbound  = revm_lookup_index(supstr);
  
  /* Bound check */
  if (infbound >= upbound)
    {
      if (!world.curjob->curcmd->endlabel)
	cmd_quit();
      world.curjob->iter[world.curjob->curloop].listidx = REVM_IDX_UNINIT;
      revm_move_pc(world.curjob->curcmd->endlabel);
      if (world.curjob->curloop)
	world.curjob->curloop--;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* Set the induction variable */
  if (!induction->type || induction->type->type != ASPECT_TYPE_INT)
    revm_convert_object(induction, ASPECT_TYPE_INT);
  if (!induction->type || induction->type->type != ASPECT_TYPE_INT)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid type for induction variable", -1);
  
  var->immed = 1;
  var->immed_val.word = infbound++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}


/** Retreive the iteration object */
static void	*revm_iterator_get(char *itername, char ***keys, int *keynbr, char *tableorlist)
{
  char		*setname;
  revmexpr_t	*setexpr;
  revmobj_t	*setobj;
  hash_t	*table;
  list_t	*list;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First try to lookup the container name */
  table = NULL;
  list = NULL;
  setname = NULL;
  setexpr = revm_lookup_param(itername, 1);
  setobj = (setexpr && setexpr->type ? setexpr->value : NULL);
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
				 "Unable to find table name", NULL);
		  setname = table->name;
		}
	      else if (setobj->otype->type == ASPECT_TYPE_LIST)
		{
		  list = (list_t *) (setobj->immed ? setobj->immed_val.ent : 
				     setobj->get_obj(setobj->parent));
		  if (!list)
		    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				 "Unable to find list name", NULL);
		  setname = list->name;
		}
	    }
	}
    }
  else
    setname = itername;
  if (!setname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to find iterator name", NULL);
  setname = strdup(setname);
  if (setexpr)
    revm_expr_destroy_by_name(setexpr->label);
 
#if __DEBUG_FOREACH__
  fprintf(stderr, "ITERATOR_GET: Found setname = %s in foreach ! \n", setname);
#endif

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
			   "Unable to find hash table", NULL);
	    }
	}
    }
  
  /* See if the list or hash is already being iterated */
  if (list)
    {
      if (world.curjob->iter[world.curjob->curloop].listidx == REVM_IDX_UNINIT && 
	  elist_linearity_get(list))
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Cannot iterate again on linearly typed list", NULL);
	}
      elist_linearity_set(list, 1);
      *keys = elist_get_keys(list, keynbr);
      world.curjob->iter[world.curjob->curloop].list = (void *) list;
      *tableorlist = 0;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, list);  
    }
  
  /* Else we know that we deal with a hash table */
  if (world.curjob->iter[world.curjob->curloop].listidx == REVM_IDX_UNINIT && 
      hash_linearity_get(table))
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Cannot iterate again on linearly typed table", NULL);
    }
  hash_linearity_set(table, 1);
  *keys = hash_get_keys(table, keynbr);
  world.curjob->iter[world.curjob->curloop].list = (void *) table;
  *tableorlist = 1;

  // To test
  //XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, table);  
}


/** Free the current iterator */
static void	revm_iterator_free(hash_t *table, list_t *list, char *indname)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->curcmd->endlabel)
    cmd_quit();
  if (table)
    hash_linearity_set(table, 0);
  else if (list)
    elist_linearity_set(list, 0);

#if __DEBUG_FOREACH__
  fprintf(stderr, "ITERATOR_FREE: Cleaning induction %s and decrementing curloop ! \n", indname);
#endif

  revm_expr_hide(indname);
  
  /* Decrease iteration depth */
  bzero(&world.curjob->iter[world.curjob->curloop], sizeof(revmiter_t));
  world.curjob->iter[world.curjob->curloop].listidx = REVM_IDX_UNINIT;
  world.curjob->iter[world.curjob->curloop].curind  = NULL;
  if (world.curjob->curloop)
    world.curjob->curloop--;
  revm_move_pc(world.curjob->curcmd->endlabel);	  
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/** Process the current induction variable */
static int	revm_induction_process(hash_t *table, list_t *list, char *curkey,
				       revmexpr_t *induction, char *paramname)
  
{
  void		*elem;
  u_int		typeid;
  char		*indname;
  char		*typename;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (table)
    {
      elem = hash_get(table, curkey);
      typeid = table->type;
    }
  else
    {
      elem = elist_get(list, curkey);
      typeid = list->type;
    }
  
#if __DEBUG_FOREACH__
  printf("INDUCTION_PROCESS: Got elem = %p for key = %s \n", elem, curkey);
#endif

  /* If the element is already an expression, copy it to the induction variable */
  if (typeid == ASPECT_TYPE_EXPR)
    {
      indname = strdup(induction->label);
      
#if 1
      fprintf(stderr, "INDUCTION_PROCESS: cleaning existing expr %s \n", induction->label);
#endif

      revm_expr_clean(induction->label);
      induction = revm_expr_copy((revmexpr_t *) elem, indname, 0);
      world.curjob->iter[world.curjob->curloop].curind = induction;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* If the element is not an expression, act depending on its type */
  if (!induction->type || induction->type->type != typeid)
    revm_convert_object(induction, typeid);
  if (induction->type->type != typeid)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid type for induction variable", -1);
  
  /* Update depending on simple or complex expr type */
  if (aspect_type_simple(induction->type->type) && !induction->type->next)
    {
      induction->value->immed = 1;
      induction->value->immed_val.ent = (eresi_Addr) elem;
#if __DEBUG_FOREACH__
      fprintf(stderr, "Setting indvar->immed_val.ent = %X \n", (eresi_Addr) elem);
#endif
    }
  else 
    {
      typename = induction->type->name;
      revm_expr_hide(paramname); //
      induction = revm_inform_type_addr(typename, paramname, (eresi_Addr) elem, NULL, 0, 1);
      if (!induction)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to create expression for induction variable", -1);
      world.curjob->iter[world.curjob->curloop].curind = induction;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Perform iteration on a container or an array of integers */
int		cmd_foreach()
{
  revmexpr_t	*induction;
  u_char	flag;
  char		**keys;
  int		keynbr;
  hash_t	*table;
  list_t	*list;
  void		*container;
  int		index;
  char		tableorlist;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  tableorlist = 0;

#if __DEBUG_FOREACH__
  fprintf(stderr, 
	  "Beginning CMD_FOREACH : curcmd->name = %s curcmd->end = %s "
	  "iter[cur].end = %s curloop = %u curscope = %u\n",
	  world.curjob->curcmd->name, 
	  world.curjob->curcmd->endlabel, 
	  world.curjob->iter[world.curjob->curloop].end,
	  world.curjob->curloop, world.curjob->curscope);
#endif

  /* Depends the mode we are in */
  flag = (world.curjob->curcmd->argc == 3   ? 1 : 
	  world.curjob->curcmd->use_regx[0] ? 2 : 3);

  /* Create or get the induction variable */
  induction = revm_induction_get(world.curjob->curcmd->param[0]);
  if (!induction)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to initialize induction variable", -1);

  /* Perform subcommand */
  switch (flag)
    {
      /* Syntax: foreach elmvar of list/hash [as regx] */
    case 1:
    case 2:
      container = revm_iterator_get(world.curjob->curcmd->param[2], &keys, &keynbr, &tableorlist);
      if (!container)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to get iterator for container", -1);
      switch (tableorlist)
	{
	case 0:
	  list  = (list_t *) container;
	  table = NULL;
	  break;
	case 1:
	  table = (hash_t *) container;
	  list  = NULL;
	  break;
	}
      
      /* Use the correct keys array index depending on foreach iteration nbr */
    nextelem:
      if (world.curjob->iter[world.curjob->curloop].listidx == REVM_IDX_UNINIT)
	{
	  index = 0;
	  world.curjob->iter[world.curjob->curloop].listidx += 2;
	}
      
      /* The induction variable existed already : record its previous modification in container */
      else
	{
	  index = world.curjob->iter[world.curjob->curloop].listidx++;
	  revm_induction_record(flag, induction, keys[index - 1], table, list);
	}

      /* Bound check : go to the loop end when we finished to iterate on the whole list/hash */
      if (index >= keynbr)
	{
	  revm_iterator_free(table, list, induction->label);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
      world.curjob->iter[world.curjob->curloop].curkey   = keys[index]; 

      /* Get the next elem if the current one is not matching */
      if (flag == 2 && regexec(&world.curjob->curcmd->regx[0], 
			       keys[index], 0, 0, 0))
	goto nextelem;

      /* Process the current element */
      if (revm_induction_process(table, list, keys[index], induction, 
				 world.curjob->curcmd->param[0]) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to iterate on induction variable", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      
      /* Syntax: foreach idxvar of minint until maxint */
    case 3:
      if (revm_loop_array(induction, 
			  world.curjob->curcmd->param[2], 
			  world.curjob->curcmd->param[4]) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to iterate on array", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      
      /* default case: syntax error */
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid foreach parameters", -1);
    }
  
  /* Support when until goes decreasing */
  /* Here set the induction variable (3 cases : first, iterating, last) */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** End an iteration */
int		cmd_forend()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (world.curjob->recur[world.curjob->curscope].rwrt.transformed &&
      world.curjob->recur[world.curjob->curscope].rwrt.idloop == world.curjob->curloop)
    {
      elist_destroy(world.curjob->recur[world.curjob->curscope].rwrt.transformed);
      world.curjob->recur[world.curjob->curscope].rwrt.transformed = NULL;
      world.curjob->recur[world.curjob->curscope].rwrt.idloop = 0;
    }

  revm_move_pc(world.curjob->curcmd->endlabel);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
