/**
* @file libstderesi/analysis/match.c
** @ingroup analysis
** @brief Implementation of program transformation in ERESI
**
** Start on Wed May 23 13:55:45 2007 jfv
**
** $Id$
*/
#include "libstderesi.h"


/** 
 * Retreive the annotation for a given expression
 * @param name Expression name
 * @return A pointer on the annotation, or NULL if failed.
 */
revmannot_t	*revm_annot_get(char *name)
{
  revmexpr_t	*expr;
  aspectype_t	*type;
  hash_t	*thash;
  revmannot_t	*annot;
  char		newname[BUFSIZ] = {0x00};

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  expr = revm_expr_get(name);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input expression name", NULL);
  type = expr->type;
  snprintf(newname, sizeof(newname), "type_%s", type->name);
  thash = hash_find(newname);
  annot = hash_get(thash, name);
  if (!annot)
    {
      //fprintf(stderr, "UNABLE TO FIND ANNOT FOR : NAME = %s \n", name);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to find annotation for expression", NULL);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, annot);
}


/** 
 * Copy a field value from one expression to another (provided destination indeed has that field)
 * @param dest Destination expression to add field to
 * @param source Source expression tocopy field from
 * @param fname Name of field to copy from source to destination
 * @return 0 for success and -1 for error
 */
static int	revm_field_propagate(revmexpr_t *dest, revmexpr_t *source, char *fname)
{
  char		srcname[BUFSIZ];
  char		dstname[BUFSIZ];
  revmexpr_t	*dst;
  revmexpr_t	*child;
  revmannot_t	*annot;
  revmannot_t	*dstannot;
  revmannot_t	*addedannot;
  char		*newdata;
  /*eresi_Addr	addr; */

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Propagate input links */
  snprintf(srcname, sizeof(srcname), "%s.%s", source->label, fname);
  child = revm_expr_get(srcname);
  if (!child)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  snprintf(dstname, sizeof(dstname), "%s.%s", dest->label, fname);
   dst = revm_expr_get(dstname);

  /* If destination expression has no such field, create it now */
  if (!dst)
    {
      annot = revm_annot_get(child->label); // annot de lexpr a copier
      dstannot = revm_annot_get(dest->label); // annot de lexpr mere destination
      if (!annot || !dstannot)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed to lookup annotations for input expressions", -1);

      XREALLOC(__FILE__, __FUNCTION__, __LINE__, newdata, 
	       (char *) dstannot->addr, dest->type->size + child->type->size, -1);
      dstannot->addr = (eresi_Addr) newdata;

      dst = revm_expr_copy(child, dstname, 1);
      dst->next = dest->childs;
      dest->childs = dst;
      
      addedannot = revm_annot_get(dstname);
      if (!addedannot)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed to add annotation on new field", -1);
      memcpy((char *) dstannot->addr + dest->type->size, (char *) addedannot->addr, child->type->size);
      addedannot->addr = dstannot->addr + dest->type->size;

      /* XXX-FIXME: annotations needs to be updated for childs of this type too.... */
      /*
	dest->type->size += child->type->size;
	revm_inform_type_addr(dst->type->name, dstname, dstannot->addr + dest->type->size, dst, 0, 0);
      */
    }

  /* Else assign it */
  else if (child->value && revm_object_set(dest, child) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to copy expression field", -1);
  else if (revm_expr_set(dst, child) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to copy expression fields", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);    
}


/** 
 * Propagate the link between 2 objects 
 * @param dest  Destination expression to copy links to
 * @param source Source expression to copy links from
 * @return 0 for success and -1 for error
 */
static int	revm_links_propagate(revmexpr_t *dest, revmexpr_t *source)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (revm_field_propagate(dest, source, "inlinks") < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to copy expression INLINKS field", -1);
  if (revm_field_propagate(dest, source, "outlinks") < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to copy expression OUTLINKS field", -1);
  if (revm_field_propagate(dest, source, "nbrinlinks") < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to copy expression NBRINLINKS field", -1);
  if (revm_field_propagate(dest, source, "nbroutlinks") < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to copy expression NBROUTLINKS field", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Perform the transformation (can be called from case or into commands) 
 * @param matchme
 * @param destvalue
 * @return
*/
static int	revm_case_transform(revmexpr_t *matchme, char *destvalue)
{
  u_int		dstnbr;
  char		*curptr;
  char		*foundptr;
  u_int		curidx;
  list_t	*exprlist;
  aspectype_t	*type;
  char		namebuf[BUFSIZ];
  char		*rname;
  revmexpr_t	*candid;
  list_t	*looplist;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We matched : first find how many elements there is in the target (list) type */
  exprlist = world.curjob->recur[world.curjob->curscope].rwrt.transformed;
  dstnbr = 1;

  for (curidx = world.curjob->iter[world.curjob->curloop].listidx - 1, curptr = destvalue; 
       curptr && *curptr; 
       curptr = foundptr + 2, curidx++, dstnbr++)
    {
      foundptr = strstr(curptr, "::");
      if (!foundptr && dstnbr == 1)
	break;
      if (foundptr)
	*foundptr = 0x00;
      type   = revm_exprtype_get(curptr);
      snprintf(namebuf, BUFSIZ, "%s-%u", world.curjob->iter[world.curjob->curloop].curkey, curidx); 
      rname = strdup(namebuf);
      candid = revm_expr_create(type, rname, curptr);
      if (!candid || !candid->annot)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid target type(s) for transformation", -1);
      candid->annot->inhash = 1;
      elist_append(exprlist, rname, candid);
      if (!foundptr)
	break;
    }

  /* UNIMPLEMENTED: Case where the rewritten element is not part of an iterated list -- unsupported */
  looplist = (list_t *) world.curjob->iter[world.curjob->curloop].list;
  if (!looplist)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Rewriting of non-list element unimplemented", -1);
  else if (looplist->type != ASPECT_TYPE_EXPR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Rewrite of non-expression variables unimplemented", -1);
  
  /* Simply replace the current list element now */
  /* The type of the list (list_t->type) does not change : it still is a list of revmexpr_t */

  /* Just one element to swap */
  else if (dstnbr == 1)
    {

      /* No transformation, keep the original expression */
      if (!strcmp(destvalue, "."))
	candid = matchme;
      else
	{

	  /* Case where we did not have a simple value here but a real expression */
	  rname = revm_tmpvar_create();	  
	  type   = revm_exprtype_get(destvalue);
	  candid = revm_expr_create(type, rname, destvalue);
	  if (!candid || !candid->annot)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Malformed destination type", -1);
	  candid->annot->inhash = 1;

	  /*
	    XXX: Disabled -- do not remove -- to renable when SSA transform is ready
	    if (revm_links_propagate(candid, matchme) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	    "Error while propagating dataflow links", -1);
	  */

	  elist_set(looplist, strdup(world.curjob->iter[world.curjob->curloop].curkey), candid);
	  elist_append(exprlist, strdup(rname), candid);
	}
    }

  /* Insert a list at a certain offset of the list and remove matched element */
  else
    {
      elist_replace(looplist, world.curjob->iter[world.curjob->curloop].curkey, 
		    elist_copy(exprlist, ELIST_DATA_NOCOPY));
      world.curjob->iter[world.curjob->curloop].listidx += exprlist->elmnbr - 1;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Execute side-effects command at some transformation point 
 * @param str
 */
static int	revm_case_execmd(char *str)
{
  revmargv_t	*curcmd;
  char          actual[26];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->curscope++;
  snprintf(actual, sizeof(actual), "job%u_rec%u_labels", world.curjob->id, world.curjob->curscope);
  hash_init(&world.curjob->recur[world.curjob->curscope].labels, 
	    strdup(actual), 11, ASPECT_TYPE_STR);
  snprintf(actual, sizeof(actual), "job%u_rec%u_exprs", world.curjob->id, world.curjob->curscope);
  hash_init(&world.curjob->recur[world.curjob->curscope].exprs, strdup(actual), 1, ASPECT_TYPE_EXPR);

  curcmd = world.curjob->curcmd;

  if (revm_exec_str(str) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Side-effects preparation failed", -1);

  world.curjob->curcmd = world.curjob->recur[world.curjob->curscope].script;
  if (revm_execmd() < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Side-effects execution failed", -1);
  world.curjob->curcmd = curcmd;

  world.curjob->recur[world.curjob->curscope].script = NULL;
  hash_destroy(&world.curjob->recur[world.curjob->curscope].labels);

  hash_destroy(&world.curjob->recur[world.curjob->curscope].exprs);

  world.curjob->curscope--;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Translate in destination type 
 */
int		cmd_into()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->recur[world.curjob->curscope].rwrt.matched)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (world.curjob->recur[world.curjob->curscope].rwrt.replaced)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot transform a second time", -1);
  if (!world.curjob->recur[world.curjob->curscope].rwrt.matchexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot transform outside a rewrite", -1);
  if (revm_case_transform(world.curjob->recur[world.curjob->curscope].rwrt.matchexpr, 
			  strdup(world.curjob->curcmd->param[0])) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to transform expression", -1);
  world.curjob->recur[world.curjob->curscope].rwrt.replaced = 1;
  if (!world.state.revm_quiet)
    revm_output(" [*] Expression transformed succesfully \n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Perform pre-side-effects 
 */
int		cmd_pre()
{
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->recur[world.curjob->curscope].rwrt.matched)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (world.curjob->recur[world.curjob->curscope].rwrt.replaced)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot perform pre-side-effects after transformation", -1);
  if (!world.curjob->recur[world.curjob->curscope].rwrt.matchexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Command cannot execute outside a rewrite", -1);
  str = revm_string_get(world.curjob->curcmd->param);
  if (revm_case_execmd(str) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "PRE side-effects command failed", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Perform pre-side-effects 
 */
int		cmd_post()
{
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->recur[world.curjob->curscope].rwrt.matched)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (!world.curjob->recur[world.curjob->curscope].rwrt.replaced)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot perform post-side-effects before transformation", -1);
  if (!world.curjob->recur[world.curjob->curscope].rwrt.matchexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Command cannot execute outside a rewrite", -1);
  str = revm_string_get(world.curjob->curcmd->param);
  if (revm_case_execmd(str) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "POST side-effects command failed", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Check input type, translate if matching 
 */
int		cmd_case()
{
  aspectype_t	*exprtype;
  revmexpr_t	*matchme;
  revmexpr_t	*candid;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->recur[world.curjob->curscope].rwrt.matchexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Case is not in a match", -1);

  exprtype = aspect_type_get_by_id(ASPECT_TYPE_EXPR);
  if (!exprtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Expression type not found : lacking reflection ?", -1);

  /* If a previous case has already matched, simply end the transformation now :
     We must do that here because some "post" commands can be put after a matching 
     "case", so we only stop rewriting at the first case -following- a matching case */
  if (world.curjob->recur[world.curjob->curscope].rwrt.matched)
    {
      revm_move_pc(world.curjob->curcmd->endlabel);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Check if we match */
  matchme = (revmexpr_t *) world.curjob->recur[world.curjob->curscope].rwrt.matchexpr;
  if (!matchme->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid type for matchme expression", -1);
  candid = revm_expr_create(matchme->type, "$candid", strdup(world.curjob->curcmd->param[0]));
  ret = (!candid ? 1 : revm_expr_match(candid, matchme));
  
  /* No match or bad match : nothing happens */
  if (ret)
    {
      world.curjob->recur[world.curjob->curscope].rwrt.matched = 0;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  

  /* Matched : transform and execute post side effects if any */
  world.curjob->recur[world.curjob->curscope].rwrt.matched = 1;

  /* Sometimes the case command comes directly with appended post side-effects */
  if (!world.curjob->curcmd->param[1])
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  revm_case_transform(matchme, strdup(world.curjob->curcmd->param[1]));
  if (world.curjob->curcmd->param[2] && 
      revm_case_execmd(world.curjob->curcmd->param[2]) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Post-side-effects commands failed", -1);
  
  /* Jump to end of the match construct */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Beginning of the transform command, open a transformation switch 
 */
int			cmd_match()
{
  list_t		*list;
  revmexpr_t		*ind;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* The first time we enter this command, we have to fetch the params */
  list = (list_t *) world.curjob->iter[world.curjob->curloop].list;
  ind  = world.curjob->iter[world.curjob->curloop].curind;

  if (!list || !ind || strcmp(ind->label, world.curjob->curcmd->param[0]) || 
      list->type != ASPECT_TYPE_EXPR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Match/Rewrite only acts on iterated lists of expressions", -1);

  /*
  if (list && ind && !strcmp(ind->label, world.curjob->curcmd->param[0]))
    {
      if (list->type != ASPECT_TYPE_EXPR)

#if __DEBUG_REWRITE__
      fprintf(stderr, "\n [DEBUG] We -ARE- matching elements of a list *** \n");
#endif

      world.curjob->recur[world.curjob->curscope].rwrt.matchexpr = ind;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

#if __DEBUG_REWRITE__
  fprintf(stderr, "\n [DEBUG] We are -NOT- matching elements of a list *** \n");
#endif    

  ind = revm_lookup_param(world.curjob->curcmd->param[0], 1);
  world.curjob->recur[world.curjob->curscope].rwrt.matchexpr = ind;
  */

  world.curjob->recur[world.curjob->curscope].rwrt.matchexpr = ind;
  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 world.curjob->recur[world.curjob->curscope].rwrt.transformed, sizeof(list_t), -1);
  elist_init(world.curjob->recur[world.curjob->curscope].rwrt.transformed, 
	     "transformed", ASPECT_TYPE_EXPR);
  world.curjob->recur[world.curjob->curscope].rwrt.idloop = world.curjob->curloop;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * End of match. Do nothing. 
 */
int		cmd_matchend()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //elist_destroy(world.curjob->recur[world.curjob->curscope].rwrt.transformed);
  //bzero(&world.curjob->recur[world.curjob->curscope].rwrt, sizeof(revmrewrite_t));

  world.curjob->recur[world.curjob->curscope].rwrt.matchexpr = NULL;
  world.curjob->recur[world.curjob->curscope].rwrt.matched = 0;
  world.curjob->recur[world.curjob->curscope].rwrt.replaced = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Default case of a match when nothing else has matched 
 */
int		cmd_default()
{
  char		*str;
  revmargv_t	*cur;
  char          actual[ERESI_MEANING];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  cur = world.curjob->curcmd;

  /* Dont execute if we matched something */
  if (world.curjob->recur[world.curjob->curscope].rwrt.matched)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Create new context */
  world.curjob->curscope++;
  snprintf(actual, sizeof(actual), "job%u_rec%u_labels", 
	   world.curjob->id, world.curjob->curscope);
  hash_init(&world.curjob->recur[world.curjob->curscope].labels, strdup(actual), 3, ASPECT_TYPE_UNKNOW);
  snprintf(actual, sizeof(actual), "job%u_rec%u_exprs", 
	   world.curjob->id, world.curjob->curscope);
  hash_init(&world.curjob->recur[world.curjob->curscope].exprs, strdup(actual), 7, ASPECT_TYPE_UNKNOW);

  /* Execute parameter commands */
  str = revm_string_get(world.curjob->curcmd->param);
  cur = world.curjob->curcmd;
  if (revm_exec_str(str) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Display execrequest failed", -1);
  world.curjob->curcmd = world.curjob->recur[world.curjob->curscope].script; 
  if (revm_execmd() < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Default command execution failed", -1);

  /* Restore previous context */
  world.curjob->curcmd = cur;
  world.curjob->recur[world.curjob->curscope].script = NULL;
  hash_destroy(&world.curjob->recur[world.curjob->curscope].labels);
  hash_destroy(&world.curjob->recur[world.curjob->curscope].exprs);
  world.curjob->curscope--;

  /* Jump to end of rewrite construct */
  revm_move_pc(world.curjob->curcmd->endlabel);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
