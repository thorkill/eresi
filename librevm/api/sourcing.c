/**
** @file sourcing.c
** 
** @ingroup librevm_api
*/
/*
** Started on  Fri May 16 15:18:35 2005 jfv
** $Id: sourcing.c,v 1.4 2008-02-16 12:32:27 thor Exp $
*/
#include "revm.h"


/** 
 * @brief Command interface for ERESI script sourcing
 *
 * @param params
 * @return
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
  char          actual[26];
  char		framename[26];
  int		argc;
  int		ret;
  revmobj_t	*new;
  revmexpr_t	*prevargc;
  revmexpr_t	*expr;
  revmexpr_t	*param;
  list_t	*newframe;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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
  if (!strstr(str, REVM_CONFIG) && world.state.revm_mode == REVM_STATE_DEBUGGER)
    world.state.revm_sourcing = 1;

  /* Open the script file and pass its parameters */
  argv = alloca((2 + argc) * sizeof(char *));
  argv[0] = "#!elfsh";
  argv[1] = str;
  if (!revm_testscript(2, argv))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid script file", -1);

  /* Copy all procedure parameters as new objects arguments */
  snprintf(framename, sizeof(framename), "%u", world.curjob->sourced);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newframe, sizeof(list_t), -1);
  elist_init(newframe, strdup(framename), ASPECT_TYPE_EXPR);

  /* Create new parameters and store the old ones in a frame */
  for (ac = 1; params[ac]; ac++)
    {

      /* Get the parameter value. Can be a previous function parameter ($Num) itself 
	 -> lookup has to be done before creating the new parameter */
      expr = revm_lookup_param(params[ac]);

      /* Then save the previous $1...N variables */
      argv[ac + 1] = params[ac]; 
      snprintf(actual, sizeof(actual), "$%u", ac);
      param = revm_expr_get(actual);
      if (param)
	{
	  elist_add(newframe, strdup(actual), (void *) param);
	  hash_del(&exprs_hash, (char *) actual);

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Parameter %s stacked with type = %s \n", 
	  actual, (param->type ? param->type->name : "UNKNOWN TYPE"));
#endif
	}

      /* Then install the new ones */
      /* This will copy the parameter expression under the name "$N"
	 and name it in the expression hash table */

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Parameter %s added with type = %s \n", 
	  actual, (expr->type ? expr->type->name : "UNKNOWN TYPE"));
#endif

      expr = revm_expr_copy(expr, actual, 0);
    }
  argv[ac + 1] = NULL;

  /* Create new argc variable and add the new frame to the list */
  prevargc = revm_expr_get(REVM_VAR_ARGC);
  if (prevargc)
    hash_del(&exprs_hash, (char *) REVM_VAR_ARGC);
  new = revm_create_IMMED(ASPECT_TYPE_INT, 1, idx - 1);
  revm_expr_create_from_object(new, REVM_VAR_ARGC);
  elist_add(&frames_list, strdup(framename), (void *) newframe);

  /* Prepare the interpreter for executing a new script */
  XOPEN(fd, str, O_RDONLY, 0, -1);
  world.curjob->sourced++;
  snprintf(actual, sizeof(actual), "job%u_labels", world.curjob->sourced);
  hash_init(&labels_hash[world.curjob->sourced], strdup(actual), 251, ASPECT_TYPE_STR);
  expr = revm_expr_get(REVM_VAR_ESHLEVEL);
  expr->value->immed_val.ent = world.curjob->sourced;

  /* Save state for restoring on "continue" */
  savedfd    = world.curjob->ws.io.input_fd;
  savedinput = world.curjob->ws.io.input;
  savedmode  = world.state.revm_mode;
  savedcmd   = world.curjob->curcmd;

  /* Temporary takes input from the script file */
  revm_setinput(&world.curjob->ws, fd);
  world.curjob->ws.io.input_fd = fd;
  world.state.revm_mode       = REVM_STATE_SCRIPT;
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
  world.curjob->curcmd = world.curjob->script[world.curjob->sourced];

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
      if (prevargc)
	hash_set(&exprs_hash, strdup((char *) REVM_VAR_ARGC), prevargc);
      expr->value->immed_val.ent = world.curjob->sourced;
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
  list_t	*lastframe;
  char		buf[BUFSIZ];
  unsigned int		idx;
  char		**keys;
  int		keynbr;
  revmexpr_t	*expr;

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

  /* Link back all parameters and their types */
  snprintf(buf, BUFSIZ, "%u", world.curjob->sourced - 1);
  lastframe      = elist_get(&frames_list, buf);
  if (!lastframe)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  keys = elist_get_keys(lastframe, &keynbr);
  for (idx = 0; idx < keynbr; idx++)
    {
      expr = elist_get(lastframe, keys[idx]);
      hash_set(&exprs_hash    , keys[idx], expr);

#if __DEBUG_EXPRS__
      fprintf(stderr, " [D] Parameter %s restored with type = %s \n", 
	      keys[idx], expr->type->name);
#endif

    }

  /* Destroy frame */
  elist_destroy(lastframe);
  elist_del(&frames_list, buf);
  elist_free_keys(keys);

  world.curjob->script[world.curjob->sourced] = NULL;
  hash_destroy(&labels_hash[world.curjob->sourced]);
  world.curjob->sourced--;
  
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
  av = revm_doargv(nbr, (unsigned int *)&ac, str);
  if (revm_parseopt(ac, av) < 0)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__,av);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Command failed", (-1));
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
