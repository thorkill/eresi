/**
** @file match.c 
**
** @brief Implementation of program transformation in ERESI
**
** Start on Wed May 23 13:55:45 2007 jfv
**
** $Id: match.c,v 1.1 2007-11-29 14:01:56 may Exp $
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
  if (!hasarrow || !*inputstr || !*outputstr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid format for parameters", -1);
  if (hasqmark && !*cmdstr)
    hasqmark = 0;
  cmdargs->param[0] = aproxy_strdup(inputstr);
  cmdargs->param[1] = aproxy_strdup(outputstr);
  cmdargs->param[2] = (hasqmark ? aproxy_strdup(cmdstr) : NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, hasqmark);
}



/** Check input type, translate if matching */
int		cmd_case()
{
  container_t	*container;
  revmexpr_t	*matchme;
  revmexpr_t	*candid;
  int		ret;
  char		*curptr;
  char		*foundptr;
  list_t	exprlist;
  aspectype_t	*type;
  aspectype_t	*exprtype;
  char		namebuf[BUFSIZ];
  char		*rname;
  u_int		curidx;
  u_int		qmark;
  u_int		dstnbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!world.curjob->matchexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Case is not in a match", -1);

  /* The transformed element is not part of any list ! */
  if (!world.curjob->itlist)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unsupported use of transform command without list parameter", -1);

  exprtype = aspect_type_get_by_name("revmexpr_t");
  if (!exprtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Expression type not found : lacking reflection ?", -1);

  /* Call a small parser for parameters */
  qmark = revm_case_format(world.curjob->curcmd);
  if (qmark < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid syntax for case parameters", -1);

  /* If type matched, do translation */
  matchme = (revmexpr_t *)  world.curjob->matchexpr->data;
  type    = (aspectype_t *) matchme->type;
  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid type for matchme expression", -1);
  candid = revm_expr_create(type, "$candid", world.curjob->curcmd->param[0]);
  ret    = revm_expr_match(matchme, candid);
  
  /* No match or bad match : nothing happens */
  if (ret)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* We matched : first find how many elements there is in the target (list) type */
  dstnbr = 1;
  list_init(&exprlist, "curdestlist", ASPECT_TYPE_EXPR);
  for (curidx = *world.curjob->curlistidx - 1, curptr = world.curjob->curcmd->param[1]; 
       curptr && *curptr; 
       curptr = foundptr + 2, curidx++, dstnbr++)
    {
      foundptr = strstr(curptr, "::");
      if (!foundptr)
	break;
      *foundptr = 0x00;
      type   = revm_exprtype_get(curptr);
      snprintf(namebuf, BUFSIZ, "%s-%u", world.curjob->matchexprname, curidx); 
      rname = aproxy_strdup(namebuf);
      candid = revm_expr_create(type, rname, curptr);
      if (!candid)
	{
	  list_destroy(&exprlist);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Malformed destination expression", -1);
	}	

      /* Create container and put expr in, with the same links */
      container = container_create(exprtype->type, candid, 
				   world.curjob->matchexpr->inlinks,
				   world.curjob->matchexpr->outlinks);
      list_add(&exprlist, rname, container);
    }
    
  /* Simply replace the current list element now */
  /* The type of the list (list_t->type) does not change : it still is a list of revmexpr_t */

  /* Just one element to swap */
  if (dstnbr == 1)
    {
      list_destroy(&exprlist);

      /* No transformation, keep the original expression */
      if (!strcmp(world.curjob->curcmd->param[1], "."))
	candid = matchme;
      else
	{
	  type   = revm_exprtype_get(world.curjob->curcmd->param[1]);
	  candid = revm_expr_create(type, aproxy_strdup(world.curjob->matchexprname), 
				    world.curjob->curcmd->param[1]);
	  if (!candid)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Malformed destination type", -1);
	}

      /* Create container and put expr inn, with the same links */
      container = container_create(exprtype->type, candid, 
				   world.curjob->matchexpr->inlinks,
				   world.curjob->matchexpr->outlinks);
      list_set(world.curjob->itlist, aproxy_strdup(world.curjob->matchexprname), container);
    }

  /* Insert a list at a certain offset of the list */
  else
    {
      list_replace(world.curjob->itlist, world.curjob->matchexprname, &exprlist);
      *world.curjob->curlistidx += exprlist.elmnbr - 1;
      list_destroy(&exprlist);
    }
  
  /* FIXME: Now execute commands of that case if any */
  if (qmark)
    revm_output(" [D] Commands in a case are not yet supported, but case was performed\n");

  /* Jump to end of the match construct */
  revm_move_pc(world.curjob->curcmd->endlabel);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** Begining of the transform command, open a transformation switch */
int			cmd_match()
{
  revmexpr_t		*matchme;
  revmexpr_t		*param;
  int			index;
  static list_t		*list = NULL;
  static listent_t	*ent = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  matchme = NULL;

  /* The first time we enter this command, we have to fetch the params */
  if (!list)
    {
      param = revm_lookup_param(world.curjob->curcmd->param[0]);
      if (!param || !param->value)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid input expression for match", -1);
      if (param->type->type != ASPECT_TYPE_LIST)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Transformation only acts on list", -1);

      if (param->value->kname)
	list = list_find(param->value->hname);
      else
	list = list_find(param->value->kname);
      if (!list || list->type != ASPECT_TYPE_EXPR)
	{
	  list = NULL;
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid input expressions list", -1);
	}

      /* Configure the list to make it the "currently iterated" one */
      list_linearity_set(list, 1);
      world.curjob->itlist = list;
    }
  
  /* Select current element to transform */
  if (world.curjob->curcmd->listidx == REVM_IDX_UNINIT)
    {
      index = 0;
      world.curjob->curcmd->listidx += 2;
    }
  else
    index = world.curjob->curcmd->listidx++;
  
  /* Check if we are at the end of the list */
  if (index >= list->elmnbr)
    {
      if (!world.curjob->curcmd->endlabel)
	cmd_quit();
      list_linearity_set(list, 0);
      revm_move_pc(world.curjob->curcmd->endlabel);
      list = NULL;
      ent = NULL;
      world.curjob->matchexpr     = NULL;
      world.curjob->matchexprname = NULL;

      hash_del(&exprs_hash, "$matchme");

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  ent                         = (!ent ? list->head : ent->next);
  world.curjob->matchexpr     = (container_t *) ent->data;
  world.curjob->matchexprname = ent->key;

  hash_set(&exprs_hash, "$matchme",
	   (revmexpr_t *) world.curjob->matchexpr->data);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* End of match. Do nothing. */
int		cmd_matchend()
{
  char		*str;
  revmargv_t	*cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  str = revm_string_get(world.curjob->curcmd->param);
  if (revm_exec_str(str) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Exec-on-endtransf failed", -1);

  /* Execute parameter command */
  cur = world.curjob->curcmd;
  world.curjob->curcmd = world.curjob->script[world.curjob->sourced]; 
  if (revm_execmd() < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Default command execution failed", -1);
  world.curjob->curcmd = cur;

  /* Simply comes back to the opening command of the construct */
  revm_move_pc(world.curjob->curcmd->endlabel);
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
