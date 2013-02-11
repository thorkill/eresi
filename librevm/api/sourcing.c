/**
* @file librevm/api/sourcing.c
** 
** @ingroup api
*/
/*
** Started on  Fri May 16 15:18:35 2005 jfv
** $Id$
*/
#include "revm.h"


/** 
 * @brief Command interface for ERESI script sourcing.
 * @param params NULL terminated array of script parameters.
 * @return Success (0) or Error (-1).
 */
int		revm_source(char **params)
{
  char		*str;
  int		fd;
  int		savedfd;
  char		savedmode;
  revmargv_t	*savedcmd;
  char          *(*savedinput)();  
  char		**argv;
  char		**av;
  int		ac;
  int           idx;
  char          actual[40];
  int		argc;
  int		ret;
  revmobj_t	*new;
  revmexpr_t	*expr;
  revmexpr_t	*eparam[10] = {NULL};

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Make sure we still can recurse */
  if (world.curjob->curscope + 1 >= REVM_MAXSRCNEST)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot recurse anymore", (-1));

  /* build argc */
  for (argc = idx = 0; params[idx] != NULL; idx++)
    argc++;
  if (!idx)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid parameter", (-1));

  /* Resolve the parameter */
  str = revm_lookup_string(params[0]);
  if (!str)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameter", (-1));

  /* Switch to debugger script mode when sourcing */
  /* This does not include execution of .eresirc in the debugger */
  if (!strstr(str, REVM_CONFIG) && world.state.revm_mode == REVM_STATE_EMBEDDED)
    world.state.revm_sourcing = 1;

  /* Open the script file and pass its parameters */
  argv = alloca((2 + argc) * sizeof(char *));
  argv[0] = "#!elfsh";
  argv[1] = str;
  if (!revm_testscript(2, argv))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid script file", -1);

  /* Prepare the interpreter for executing a new script */
  XOPEN(fd, str, O_RDONLY, 0, -1);
  world.curjob->curscope++;
  snprintf(actual, sizeof(actual), "job%u_rec%u_labels", world.curjob->id, world.curjob->curscope);
  hash_init(&world.curjob->recur[world.curjob->curscope].labels, strdup(actual), 23, ASPECT_TYPE_STR);
  snprintf(actual, sizeof(actual), "job%u_rec%u_exprs", world.curjob->id, world.curjob->curscope);
  hash_init(&world.curjob->recur[world.curjob->curscope].exprs, strdup(actual), 23, ASPECT_TYPE_EXPR);
  world.curjob->recur[world.curjob->curscope].funcname = str;

  /* Lookup script parameters */
  for (ac = 1; params[ac]; ac++)
    {
      expr = revm_lookup_param(params[ac], 1);
      if (!expr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid script parameter", -1);
      eparam[ac - 1] = expr;
    }

  /* Create new parameters and store them in new scope */
  for (ac = 0; eparam[ac]; ac++)
    {
      snprintf(actual, sizeof(actual), "$%u", ac + 1);

#if __DEBUG_EXPRS__
      fprintf(stderr, " [D] Parameter %s added with type = %s \n", 
	      actual, (eparam[ac]->type ? eparam[ac]->type->name : "UNKNOWN TYPE"));
#endif
      
      revm_expr_copy(eparam[ac], actual, 0);
    }

  /* Change ARGC variable */
  argv[ac + 1] = NULL;
  new = revm_create_IMMED(ASPECT_TYPE_INT, 1, idx - 1);
  revm_expr_create_from_object(new, REVM_VAR_ARGC, 0);

  /* Update current recursion variable */
  expr = revm_expr_get(REVM_VAR_ESHLEVEL);
  expr->value->immed_val.ent = world.curjob->curscope;

  /* Save state for restoring on "continue" */
  savedfd    = world.curjob->ws.io.input_fd;
  savedinput = world.curjob->ws.io.input;
  savedmode  = world.state.revm_mode;
  savedcmd   = world.curjob->curcmd;

  /* Temporary takes input from the script file */
  revm_setinput(&world.curjob->ws, fd);
  world.curjob->ws.io.input_fd = fd;
  world.state.revm_mode        = REVM_STATE_SCRIPT;
  world.curjob->ws.io.input    = revm_stdinput;

  /* First we parse the script file */
  do {
    av = revm_input(&ac, NULL);
    if (av == ((char **) REVM_INPUT_VOID))
      continue;
    else if (!av || !world.curjob->curcmd)
      {
	world.state.revm_sourcing = 0;
	break;
      }
    if (revm_parseopt(ac, av) < 0)
      {
	XFREE(__FILE__, __FUNCTION__, __LINE__,av);
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid script", (-1));
      }
  } while (1);
  
  /* We then execute the parsed script */
  world.curjob->curcmd = world.curjob->recur[world.curjob->curscope].script;

  /* If we are executing a debugger script, do not clear everything yet */
  switch (revm_execscript())
    {
    case REVM_SCRIPT_CONTINUE:
      world.context.savedfd    = savedfd;
      world.context.savedmode  = savedmode;
      world.context.savedcmd   = savedcmd;
      world.context.savedinput = savedinput;
      world.context.savedargv  = argv;
      world.context.savedname  = str;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, REVM_SCRIPT_CONTINUE);
    case (-1):
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Failed to execute script", (-1));
      
      /* Restore the context immediately if no continue is met */
    default:
      ret = revm_context_restore(savedfd, savedmode, savedcmd, savedinput, argv, str);
      expr->value->immed_val.ent = world.curjob->curscope;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
    }
  
}



/**
 * Restore the previous debugger control context.
 * Mostly useful when using 'continue' command from a debugger script 
 *
 * @param savedfd
 * @param savedmode
 * @param savedcmd
 * @param savedinput
 * @param argv
 * @param savedname
 * @return
 */
int	       revm_context_restore(int		savedfd, 
				    char	savedmode, 
				    revmargv_t	*savedcmd, 
				    void	*savedinput, 
				    char	**argv,
				    char	*savedname)
{
  char		buf[BUFSIZ];
  int		idx;
  char		**keys;
  int		keynbr;
  revmexpr_t	*res;
  revmexpr_t	*oldres;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Restore modified control context */
  world.curjob->ws.io.input_fd = savedfd;
  world.state.revm_mode        = savedmode;
  world.curjob->curcmd         = savedcmd;
  world.curjob->ws.io.input    = savedinput;
  if (!world.state.revm_quiet)
    {
      snprintf(buf, BUFSIZ, "\n [*] %s sourcing -OK- \n", savedname);
      revm_output(buf);
    }

  /* Destroy current frame */
  keys = hash_get_keys(&world.curjob->recur[world.curjob->curscope].exprs, &keynbr);
  for (idx = 0; idx < keynbr; idx++)
    {

      /* Only free top-level expressions, the rest is freed recursively by revm_expr_destroy_by_name() */
      if (strstr(keys[idx], REVM_SEP))
	continue;
      
      /* Special treatment for the return value that needs to move to the inner scope */
      if (!strcmp(keys[idx], REVM_VAR_RESULT) && world.curjob->curscope)
	{
	  res = revm_expr_get(keys[idx]);
	  oldres = hash_get(&world.curjob->recur[world.curjob->curscope - 1].exprs, REVM_VAR_RESULT);
	  if (oldres)
	    revm_expr_destroy(oldres); //
	  hash_add(&world.curjob->recur[world.curjob->curscope - 1].exprs, strdup(keys[idx]), res);
	  revm_expr_print(res, 0);
	  continue;
	}

#if __DEBUG_EXPRS__
      fprintf(stderr, " [D] Expression %s will be destroyed from ending scope \n", keys[idx]);
#endif

      revm_expr_destroy_by_name(keys[idx]);

    }
  hash_free_keys(keys);

  /* Destroy frame */
  hash_destroy(&world.curjob->recur[world.curjob->curscope].labels);
  hash_destroy(&world.curjob->recur[world.curjob->curscope].exprs);
  world.curjob->recur[world.curjob->curscope].script = NULL;
  world.curjob->curscope--;
  
  /* Return OK */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Execute a command given by a string in the current job 
 *
 * @param str
 * @return
 */
int	revm_exec_str(char *str)
{
  int	nbr;
  char  **av;
  int	ac;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  nbr = revm_findblanks(str);
  av = revm_doargv(nbr, (u_int *)&ac, str);
  if (revm_parseopt(ac, av) < 0)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__,av);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Command failed", (-1));
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
