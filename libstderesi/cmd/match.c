/**
** @file match.c 
**
** @brief Implementation of program transformation in ERESI
**
** Start on Wed May 23 13:55:45 2007 jfv
**
** $Id: match.c,v 1.3 2008-02-16 12:32:27 thor Exp $
*/
#include "libstderesi.h"


/** Format arguments of case command */
static int	revm_case_format(revmargv_t *cmdargs)
{
  int		index;
  u_char	hasarrow;
  u_char	hasqmark;
  char		inputstr[BUFSIZ];
  char		outputstr[BUFSIZ];
  char		cmdstr[BUFSIZ];
  u_int		len;
  char		*curstr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (world.curjob->curcmd->argc < 3)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Missing parameters for command", -1);
  curstr = inputstr;

  /* Now pack parameters */
  for (len = hasarrow = hasqmark = index = 0; index < cmdargs->argc; index++)
    if (!strcmp(cmdargs->param[index], "->"))
      {
	hasarrow = 1;
	curstr = outputstr;
	len = 0;
      }
    else if (!strcmp(cmdargs->param[index], "?"))
      {
	hasqmark = 1;
	curstr = cmdstr;
	len = 0;
      }
    else
      len += snprintf(curstr + len, BUFSIZ - len, "%s", cmdargs->param[index]);

  /* Finished parsing and reporting potential errors */
  if (!*inputstr || (hasqmark && !hasarrow))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid format for parameters", -1);
  if (hasqmark && !*cmdstr)
    hasqmark = 0;
  if (hasarrow && !*outputstr)
    hasarrow = 0;
  cmdargs->param[0] = strdup(inputstr);
  cmdargs->param[1] = (hasarrow ? strdup(outputstr) : NULL);
  cmdargs->param[2] = (hasqmark ? strdup(cmdstr) : NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, hasqmark);
}


/* Propagate the link between 2 objects */
static int	revm_links_propagate(revmexpr_t *dest, revmexpr_t *source)
{
  char		srcname[BUFSIZ];
  char		dstname[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Propagate input links */
  snprintf(srcname, sizeof(srcname), "%s.inlinks", source->label);
  snprintf(dstname, sizeof(dstname), "%s.inlinks", dest->label);
  dest = revm_expr_get(dstname);
  source = revm_expr_get(srcname);
  if (!dest && !source)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  else if (!dest || !source)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "One expression should be in container", -1);
  else if (!dest->value || !source->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Input links list should be a pointer", -1);
  if (revm_object_set(dest, source) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to copy input links list", -1);

  /* Propagate output links */
  snprintf(srcname, sizeof(srcname), "%s.outlinks", source->label);
  snprintf(dstname, sizeof(dstname), "%s.outlinks", dest->label);
  dest = revm_expr_get(dstname);
  source = revm_expr_get(srcname);
  if (!dest || !source)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "One expression should be in container", -1);
  else if (!dest->value || !source->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Output links list should be a pointer", -1);
  if (revm_object_set(dest, source) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to copy output links list", -1);

  /* Propagate input links size */
  snprintf(srcname, sizeof(srcname), "%s.nbrinlinks", source->label);
  snprintf(dstname, sizeof(dstname), "%s.nbrinlinks", dest->label);
  dest = revm_expr_get(dstname);
  source = revm_expr_get(srcname);
  if (!dest || !source)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "One expression should be in container", -1);
  else if (!dest->value || !source->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "InLinks size should be a scalar", -1);
  if (revm_object_set(dest, source) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to copy inlinks list size", -1);

  /* Propagate output links size */
  snprintf(srcname, sizeof(srcname), "%s.nbroutlinks", source->label);
  snprintf(dstname, sizeof(dstname), "%s.nbroutlinks", dest->label);
  dest = revm_expr_get(dstname);
  source = revm_expr_get(srcname);
  if (!dest || !source)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "One expression should be in container", -1);
  else if (!dest->value || !source->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "OutLinks size should be a scalar", -1);
  if (revm_object_set(dest, source) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to copy outlinks list size", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** Perform the transformation (can be called from case or into commands) */
static int	revm_case_transform(revmexpr_t *matchme, char *destvalue)
{
  u_int		dstnbr;
  char		*curptr;
  char		*foundptr;
  u_int		curidx;
  list_t	exprlist;
  aspectype_t	*type;
  char		namebuf[BUFSIZ];
  char		*rname;
  revmexpr_t	*candid;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We matched : first find how many elements there is in the target (list) type */
  dstnbr = 1;
  elist_init(&exprlist, "curdestlist", ASPECT_TYPE_EXPR);
  for (curidx = *world.curjob->iter.curindex - 1, curptr = destvalue; 
       curptr && *curptr; 
       curptr = foundptr + 2, curidx++, dstnbr++)
    {
      foundptr = strstr(curptr, "::");
      if (!foundptr)
	break;                                                          
      *foundptr = 0x00;
      type   = revm_exprtype_get(curptr);
      snprintf(namebuf, BUFSIZ, "%s-%u", world.curjob->iter.curkey, curidx); 
      rname = strdup(namebuf);
      candid = revm_expr_create(type, rname, curptr);
      elist_add(&exprlist, rname, candid);
    }

  /* FIXME: The rewritten element is not part of any list */
  if (!world.curjob->iter.list)
    {
      if (dstnbr == 1)
	;
      else
	;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Rewriting of non-list element currently not supported", -1);
    }
    
  /* Simply replace the current list element now */
  /* The type of the list (list_t->type) does not change : it still is a list of revmexpr_t */

  /* Just one element to swap */
  else if (dstnbr == 1)
    {
      elist_destroy(&exprlist);

      /* No transformation, keep the original expression */
      if (!strcmp(destvalue, "."))
	candid = matchme;
      else
	{
	  rname  = strdup(matchme->label);
	  type   = revm_exprtype_get(destvalue);
	  revm_expr_destroy(matchme->label);
	  candid = revm_expr_create(type, rname, destvalue);
	  if (!candid)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Malformed destination type", -1);
	  if (revm_links_propagate(candid, matchme) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Error while propagating dataflow links", -1);
	  elist_set(world.curjob->iter.list, strdup(world.curjob->iter.curkey), candid);
	}
    }

  /* Insert a list at a certain offset of the list */
  else
    {
      elist_replace(world.curjob->iter.list, world.curjob->iter.curkey, &exprlist);
      *world.curjob->iter.curindex += exprlist.elmnbr - 1;
      elist_destroy(&exprlist);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Execute side-effects command at some transformation point */
static int	revm_case_execmd(char *str)
{
  revmargv_t	*curcmd;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (revm_exec_str(str) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Side-effects preparation failed", -1);
  curcmd = world.curjob->curcmd;
  world.curjob->curcmd = world.curjob->script[world.curjob->sourced];
  if (revm_execmd() < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Side-effects execution failed", -1);
  world.curjob->curcmd = curcmd;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** Translate in destination type */
int		cmd_into()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->rwrt.matched)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (world.curjob->rwrt.replaced)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot transform a second time", -1);
  if (!world.curjob->rwrt.matchexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot transform outside a rewrite", -1);
  if (revm_case_transform(world.curjob->rwrt.matchexpr, world.curjob->curcmd->param[0]) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to transform expression", -1);
  world.curjob->rwrt.replaced = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** Perform pre-side-effects */
int		cmd_pre()
{
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->rwrt.matched)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (world.curjob->rwrt.replaced)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot perform pre-side-effects after transformation", -1);
  if (!world.curjob->rwrt.matchexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Command cannot execute outside a rewrite", -1);
  str = revm_string_get(world.curjob->curcmd->param);
  if (revm_case_execmd(str) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Pre-side-effects commands failed", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** Perform pre-side-effects */
int		cmd_post()
{
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->rwrt.matched)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (!world.curjob->rwrt.replaced)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot perform post-side-effects before transformation", -1);
  if (!world.curjob->rwrt.matchexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Command cannot execute outside a rewrite", -1);
  str = revm_string_get(world.curjob->curcmd->param);
  if (revm_case_execmd(str) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Post-side-effects commands failed", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Check input type, translate if matching */
int		cmd_case()
{
  aspectype_t	*exprtype;
  revmexpr_t	*matchme;
  revmexpr_t	*candid;
  int		ret;
  u_int		qmark;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->rwrt.matchexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Case is not in a match", -1);
  exprtype = aspect_type_get_by_id(ASPECT_TYPE_EXPR);
  if (!exprtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Expression type not found : lacking reflection ?", -1);

  /* If a previous case has already matched, simply end the transformation now :
     We must do that here because some "post" commands can be put after a matching 
     "case", so we only stop rewriting at the first case -following- a matchcase */
  if (world.curjob->rwrt.matched)
    {
      revm_move_pc(world.curjob->curcmd->endlabel);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Call a small parser for parameters */
  qmark = revm_case_format(world.curjob->curcmd);
  if (qmark < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid syntax for case parameters", -1);

  /* Check if we match */
  matchme = (revmexpr_t *) world.curjob->rwrt.matchexpr;
  if (!matchme->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid type for matchme expression", -1);
  candid = revm_expr_create(matchme->type, "$candid", world.curjob->curcmd->param[0]);
  ret = (!candid ? 1 : revm_expr_match(matchme, candid));
  
  /* No match or bad match : nothing happens */
  if (ret)
    {
      world.curjob->rwrt.matched = 0;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Matched : transform and execute post side effects if any */
  world.curjob->rwrt.matched = 1;
  if (!world.curjob->curcmd->param[1])
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  revm_case_transform(matchme, world.curjob->curcmd->param[1]);
  if (world.curjob->curcmd->param[2] && revm_case_execmd(world.curjob->curcmd->param[2]) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Post-side-effects commands failed", -1);
  
  /* Jump to end of the match construct */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Beginning of the transform command, open a transformation switch */
int			cmd_match()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* The first time we enter this command, we have to fetch the params */
  if (world.curjob->iter.list && !strcmp(world.curjob->iter.curname, world.curjob->curcmd->param[0]))
    {
      if (world.curjob->iter.list->type != ASPECT_TYPE_EXPR)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Match/Rewrite can only works on expressions", -1);

      fprintf(stderr, "We -ARE- matching on a list \n");
    }
  else
    fprintf(stderr, "We are -NOT- matching on a list \n");

  world.curjob->rwrt.matchexpr = revm_lookup_param(world.curjob->curcmd->param[0]);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* End of match. Do nothing. */
int		cmd_matchend()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  bzero(&world.curjob->rwrt, sizeof(revmrewrite_t));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Default case of a match when nothing else has matched */
int		cmd_default()
{
  char		*str;
  revmargv_t	*cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  str = revm_string_get(world.curjob->curcmd->param);
  if (revm_exec_str(str) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Display execrequest failed", -1);
  
  /* Execute parameter command */
  cur = world.curjob->curcmd;
  world.curjob->curcmd = world.curjob->script[world.curjob->sourced]; 
  if (revm_execmd() < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Default command execution failed", -1);
  world.curjob->curcmd = cur;

  /* Jump to end of the match construct */
  revm_move_pc(world.curjob->curcmd->endlabel);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
