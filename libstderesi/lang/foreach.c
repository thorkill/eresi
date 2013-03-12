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
#if __DEBUG_FOREACH__
  if (world.curjob->iter[world.curjob->curloop].elmidx != REVM_IDX_UNINIT)
    {
      fprintf(stderr, 
	      "INDUCTION_LOAD: curscope = %d curloop = %d elmidx = %d reclevel = %d) named expr %s \n",
	      world.curjob->curscope, world.curjob->curloop, 
	      world.curjob->iter[world.curjob->curloop].elmidx, 
	      world.curjob->iter[world.curjob->curloop].reclevel, name);
      assert(world.curjob->iter[world.curjob->curloop].elmidx == REVM_IDX_UNINIT);
    }
#else
      assert(world.curjob->iter[world.curjob->curloop].elmidx == REVM_IDX_UNINIT);
#endif

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
  fprintf(stderr, "INDUCTION_GET: Induction variable name = %s elmidx = %u \n", name,
	  world.curjob->iter[world.curjob->curloop].elmidx);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, induction);
}



/** Record the induction variable if we have changed its state since the last iteration */
static int	revm_induction_record(revmexpr_t *induction, char *curkey, 
				      hash_t *table, list_t *list)
{
  eresi_Addr	lastvalue;
  revmobj_t	*var;
  revmexpr_t    *old = NULL;
  char          *tmpname = NULL; 

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  var = induction->value;

#if __DEBUG_FOREACH__
  fprintf(stderr, "INDUCTION_RECORD: entering with curkey = %s \n", curkey);
#endif

  /* If we have to process this element (default, or when using regex, if match) */
  if (world.curjob->iter[world.curjob->curloop].looptype != REVM_LOOP_REGEX ||
      !regexec(world.curjob->iter[world.curjob->curloop].usedregx, curkey, 0, 0, 0))
    {

      /* Case of simply typed induction variable */
      if (induction->type && aspect_type_simple(induction->type->type) && 
	  !induction->type->next)
	{
	  lastvalue = (var->immed ? var->immed_val.ent : var->get_obj(var->parent));
	  if (table) hash_set(table, curkey, (void *) lastvalue);
	  else elist_set(list, curkey, (void *) lastvalue);
#if __DEBUG_FOREACH__
	  fprintf(stderr, "INDUCTION_RECORD: back-assignment in list for elem %s (of value %X) \n", 
		  curkey, lastvalue);
#endif

	}

      /* Case of complex induction variable */
      else 
	{
	  tmpname = revm_tmpvar_create();
	  induction = revm_expr_copy(induction, tmpname, 0);
	  if (!induction)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Unable to record induction", -1);

	  fprintf(stderr, "---- PRINTING AFTER INDUCTION COPY ---- \n");
	  revm_expr_print(induction, 0);
	  fprintf(stderr, "----------------\n");

	  // FIXME: GRAB CURRENT KEY ELEMENT
	  //if (table) old = hash_get(table, curkey);
	  //else old = elist_get(list, curkey);

	  /* Replace container element at current key slot */
	  if (table) hash_set(table, curkey, induction);
	  else elist_set(list, curkey, induction);

	  // FIXME: THIS STILL HAS SOME REFERENCES ON IT - MEM LEAK?
	  // A COPY SHOULD HAVE BEEN MADE SO THAT WE COULD DESTROY OLD EXPRESSION HERE
	  //revm_expr_destroy(old);

#if __DEBUG_FOREACH__
	  fprintf(stderr, "INDUCTION_RECORD: back-assignment in list for elem %s (type EXPR) \n", curkey);
#endif

	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);  
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
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
  if (world.curjob->iter[world.curjob->curloop].elmidx == REVM_IDX_UNINIT)
    {
      infbound = revm_lookup_index(infstr);
      world.curjob->iter[world.curjob->curloop].elmidx = infbound + 1;
    }
  else
    infbound = world.curjob->iter[world.curjob->curloop].elmidx++;
  
  upbound  = revm_lookup_index(supstr);
  
  /* Bound check */
  if (infbound >= upbound)
    {
      if (!world.curjob->curcmd->endlabel)
	cmd_quit();
      world.curjob->iter[world.curjob->curloop].elmidx = REVM_IDX_UNINIT;
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
      if (world.curjob->iter[world.curjob->curloop].elmidx == REVM_IDX_UNINIT && 
	  elist_linearity_get(list))
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Cannot iterate again on linearly typed list", NULL);
	}
      elist_linearity_set(list, 1);
      *keys = elist_get_keys(list, keynbr);
      world.curjob->iter[world.curjob->curloop].container = (void *) list;
      world.curjob->iter[world.curjob->curloop].tcontainer = REVM_CONTAINER_LIST;
      *tableorlist = REVM_CONTAINER_LIST;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, list);  
    }
  
  /* Else we know that we deal with a hash table */
  if (world.curjob->iter[world.curjob->curloop].elmidx == REVM_IDX_UNINIT && 
      hash_linearity_get(table))
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, setname);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Cannot iterate again on linearly typed table", NULL);
    }
  hash_linearity_set(table, 1);
  *keys = hash_get_keys(table, keynbr);
  world.curjob->iter[world.curjob->curloop].container = (void *) table;
  world.curjob->iter[world.curjob->curloop].tcontainer = REVM_CONTAINER_HASH;
  *tableorlist = REVM_CONTAINER_HASH;

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
  world.curjob->iter[world.curjob->curloop].elmidx = REVM_IDX_UNINIT;
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
      revm_expr_hide(paramname); 
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
  u_char	looptype;
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
  looptype = (world.curjob->curcmd->argc == 3   ? REVM_LOOP_SIMPLE : 
	      world.curjob->curcmd->use_regx[0] ? REVM_LOOP_REGEX  : REVM_LOOP_RANGE);

  world.curjob->iter[world.curjob->curloop].looptype = looptype;

  /* Create or get the induction variable */
  induction = revm_induction_get(world.curjob->curcmd->param[0]);
  if (!induction)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to initialize induction variable", -1);

  /* Perform subcommand */
  switch (looptype)
    {
      /* Syntax: foreach elmvar of list/hash [as regx] */
    case REVM_LOOP_SIMPLE:
    case REVM_LOOP_REGEX:
      world.curjob->iter[world.curjob->curloop].usedregx = &world.curjob->curcmd->regx[0];
      container = revm_iterator_get(world.curjob->curcmd->param[2], &keys, &keynbr, &tableorlist);
      if (!container)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to get iterator for container", -1);
      if (!tableorlist)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to get type of container", -1);
      switch (tableorlist)
	{
	case REVM_CONTAINER_LIST:
	  list  = (list_t *) container;
	  table = NULL;
	  break;
	case REVM_CONTAINER_HASH:
	  table = (hash_t *) container;
	  list  = NULL;
	  break;
	default:
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown container type", -1);
	}
      
      /* Use the correct keys array index depending on foreach iteration nbr */
    nextelem:
      if (world.curjob->iter[world.curjob->curloop].elmidx == REVM_IDX_UNINIT)
	{
	  index = 0;
	  world.curjob->iter[world.curjob->curloop].elmidx += 2;
	}
      
      /* The induction variable existed already : record its previous modification in container */
      else
	index = world.curjob->iter[world.curjob->curloop].elmidx++;

      /* Bound check : go to the loop end when we finished to iterate on the whole list/hash */
      if (index >= keynbr)
	{
	  revm_iterator_free(table, list, induction->label);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
      world.curjob->iter[world.curjob->curloop].curkey = keys[index]; 

      /* Get the next elem if the current one is not matching */
      if (looptype == REVM_LOOP_REGEX && regexec(&world.curjob->curcmd->regx[0], keys[index], 0, 0, 0))
	goto nextelem;

      /* Process the current element */
      if (revm_induction_process(table, list, keys[index], induction, 
				 world.curjob->curcmd->param[0]) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to iterate on induction variable", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      
      /* Syntax: foreach idxvar of minint until maxint */
    case REVM_LOOP_RANGE:
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

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** End an iteration */
int		cmd_forend()
{
  list_t *list = NULL;
  hash_t *hash = NULL;
  char	typeid = 0;
  int	ret = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->iter[world.curjob->curloop].tcontainer == REVM_CONTAINER_LIST)
    {
      list = world.curjob->iter[world.curjob->curloop].container;
      typeid = list->type;
    }
  else if (world.curjob->iter[world.curjob->curloop].tcontainer == REVM_CONTAINER_HASH)
    {
      hash = world.curjob->iter[world.curjob->curloop].container;
      typeid = hash->type;
    }
  
  if (world.curjob->recur[world.curjob->curscope].rwrt.transformed &&
      world.curjob->recur[world.curjob->curscope].rwrt.idloop == world.curjob->curloop)
    {
      elist_destroy(world.curjob->recur[world.curjob->curscope].rwrt.transformed);
      world.curjob->recur[world.curjob->curscope].rwrt.transformed = NULL;
      world.curjob->recur[world.curjob->curscope].rwrt.idloop = 0;
    }

  if (world.curjob->iter[world.curjob->curloop].curind != NULL)
    {
#if __DEBUG_FOREACH__
      fprintf(stderr, 
	      "CMD_FOREND: cleaning (curscope = %d curloop = %d elmidx = %d reclevel = %d) existing expr %s \n",
	      world.curjob->curscope, world.curjob->curloop, 
	      world.curjob->iter[world.curjob->curloop].elmidx, 
	      world.curjob->iter[world.curjob->curloop].reclevel, 
	      world.curjob->iter[world.curjob->curloop].curind->label);
#endif

      /* Record induction variable in container as updates may have been performed */
      ret = revm_induction_record(world.curjob->iter[world.curjob->curloop].curind, 
				  world.curjob->iter[world.curjob->curloop].curkey, 
				  hash, list);
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Failed terminate iteration", -1);

      /* Clean induction variable from scope if we are processing list of expressions */
      /* When curelm not an expr, we dont need this "clean" call, just a "hide" call in foreach */
      if (ret == 1 && typeid == ASPECT_TYPE_EXPR)
	{
	  fprintf(stderr, "forend: cleaning induction expr \n");
	  revm_expr_hide(world.curjob->iter[world.curjob->curloop].curind->label);
	}
    }
  
  /* Start next iteration */
  revm_move_pc(world.curjob->curcmd->endlabel);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
