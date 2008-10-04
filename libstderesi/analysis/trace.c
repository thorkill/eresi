/**
* @file trace.c
* @ingroup libstderesi
* @brief All functions that help to trace content
*
* Started Jul 2 2005 00:03:44 mxatone
* $Id: trace.c,v 1.2 2008-02-16 12:32:27 thor Exp $
*
*/
#include "libstderesi.h"


/* XXX: The good syntax should be : 
** 
** traces <add>     funcname     [optional_traces_name]  : add a function to a given trace
** traces <rm>      funcname     [optional_traces_name]  : remove a function from a given trace
** traces <enable>  funcname|all [optional_traces_name]  : enable tracing for a function in a given trace (enabled by default after a add)
** traces <disable> funcname|all [optional_traces_name]  : disable tracing for a function in a given trace
** traces <create>  tracename    <optionals funcnames>   : create a new trace with a pool of traced functions by default
** traces <delete>  tracename                            : delete a trace
** traces <flush>   tracename                            : remove all functions from a given trace
** traces [optional_traces_name]                         : list all available traces
**
*/

int 			cmd_traces()
{
  tracecmd_t		*cmd;
  char			*fArg;
  char			**sArg;
  int			ret = 0;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Init tracing elements */
  etrace_init_trace();

  switch(world.curjob->curcmd->argc)
    {
    case 0:
      traces_list(world.curjob->curfile, NULL, NULL);
      break;
    case 1:
      table = etrace_get(world.curjob->curcmd->param[0]);

      if (!table)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unknown trace", -1);

      traces_list_detail(table, world.curjob->curcmd->param[0]);
      break;
    default:
      cmd = hash_get(&traces_cmd_hash, world.curjob->curcmd->param[0]);
      
      if (!cmd)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unknown command", -1);

      fArg = NULL;
      sArg = NULL;

      if (cmd->flagName > 0)
	{
	  /* First argument is needed and doesn't submited */
	  if (!world.curjob->curcmd->param[1] && cmd->flagName == 2)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "First argument is needed", -1);

	  fArg = world.curjob->curcmd->param[1];

	  if (cmd->flagArg > 0)
	    {
	      /* Second argument is needed and doesn't submited */
	      if (!world.curjob->curcmd->param[2] && cmd->flagArg == 2)
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				  "Second argument is needed", -1);

	      sArg = world.curjob->curcmd->param + 2;
	    }
	}

      ret = cmd->exec(world.curjob->curfile, fArg, sArg);      
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

int 			cmd_traceadd()
{
  unsigned int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Init tracing elements */
  etrace_init_trace();

  if (world.curjob->curcmd->argc < 1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Need at least one function", -1);

  /* Loop on each passed functions */
  for (index = 0; index < world.curjob->curcmd->argc; index++) 
    {
      if (traces_add(world.curjob->curfile, world.curjob->curcmd->param[index], NULL) < 0)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int 			cmd_traceexclude()
{
  unsigned int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->argc < 1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Need at least one function", -1);

  /* Loop on each passed functions */
  for (index = 0; index < world.curjob->curcmd->argc; index++) 
    {
      if (traces_exclude(world.curjob->curfile, world.curjob->curcmd->param[index], NULL) < 0)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen brief tag
 */
int			cmd_tracerun()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Register binary files to work on */
  elfsh_register_working_objects(&world.curjob->loaded,
				 &world.shared_hash);

  if (traces_run(world.curjob->curfile, 
		 world.curjob->curcmd->param, 
		 world.curjob->curcmd->argc) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to run current trace", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Create the structure that store trace cmd informations 
 * @param exec function pointer
 * @param flagName
 * @param flagArg
 */
tracecmd_t   *traces_create_CMDENT(int (*exec)(elfshobj_t *file, char *name, char **optarg),
				   char flagName, char flagArg)
{
  tracecmd_t *new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, new, sizeof(tracecmd_t), NULL);
  new->exec	= exec;
  new->flagName	= flagName;
  new->flagArg	= flagArg;
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/**
 * Add a command in sub commande hash table
 * @param cmd cmd name
 * @param exec function pointer
 * @param flagName
 * @param flagArg
 * @see traces_cmd_hash
 * @see traces_create_CMDENT
 */
int 		traces_addcmd(char *cmd, void *exec, char flagName, char flagArg)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  hash_add(&traces_cmd_hash, cmd, (void *) traces_create_CMDENT(exec, flagName, flagArg));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
