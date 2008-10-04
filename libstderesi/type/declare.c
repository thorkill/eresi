/*
** @file declare.c
** @ingroup libstderesi
** @brief Command used to declare a new typed variable in the environment
**
** Started on Fri Jun 22 07:23:58 2007 jfv
** $Id: declare.c,v 1.1 2007-11-29 14:01:56 may Exp $
*/
#include "libstderesi.h"



/* Declare a new typed variable */ 
int		cmd_declare()
{
  aspectype_t	*type;
  char		*varname;
  char		buf[BUFSIZ];
  unsigned int		curlen;
  unsigned int		curidx;
  unsigned int		curpidx;
  unsigned int		openbrace;
  unsigned int		closebrace;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (world.curjob->curcmd->argc < 3 ||
      strcmp(world.curjob->curcmd->param[1], "="))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid syntax for command", -1);

  /* Preliminary checks */
  type = aspect_type_get_by_name(world.curjob->curcmd->name);
  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unknown type for new variable", -1);

  /* Create the new data structure */
  bzero(buf, BUFSIZ);

  /* Create a unique string for the value of the object */
  for (openbrace = closebrace = curlen = 0, curidx = 2; 
       world.curjob->curcmd->param[curidx]; 
       curidx++, curlen += curpidx)
    for (curpidx = 0; world.curjob->curcmd->param[curidx][curpidx]; 
	 curpidx++)
      switch (world.curjob->curcmd->param[curidx][curpidx])
	{
	case '(':
	  *(buf + curlen + curpidx) = '(';
	  openbrace++;
	  break;
	case ')':
	  *(buf + curlen + curpidx) = ')';
	  closebrace++;
	  if (closebrace > openbrace)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Invalid closing structure for variable", -1);
	  break;
	case ' ':
	  *(buf + curlen + curpidx) = ',';
	  break;
	default:
	  *(buf + curlen + curpidx) = world.curjob->curcmd->param[curidx][curpidx];
	  break;
	}

  /* Some first checks on the value */
  if (closebrace != openbrace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid nesting structure for variable", -1);

#if __DEBUG_EXPRS__
  fprintf(stderr, "Expression to write in variable: %s \n", buf);
#endif

  curlen = strlen(world.curjob->curcmd->param[0]);
  varname = alloca(curlen + 2);
  snprintf(varname, curlen + 2, "$%s", world.curjob->curcmd->param[0]);

  /* Perform the real operation now */
  if (type->childs)
    {
      if (!revm_expr_create(type, varname, buf))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid value for variable", -1);
    }
  else
    {
      if (!revm_simple_expr_create(type, varname, buf))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid value for variable", -1);
    }

  /* Print success */
  if (!world.state.revm_quiet)
    {
      snprintf(buf, sizeof(buf), 
	       " [*] Variable %s succesfully initialized \n\n", 
	       world.curjob->curcmd->param[0]);
      revm_output(buf);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
