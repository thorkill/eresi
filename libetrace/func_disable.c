/**
 * @file func_disable.c
** @ingroup libetrace
** 
** @brief trace table disable functions.
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: func_disable.c,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"


/**
 * Disable a functions of a trace 
 * @param trace trace name
 * @param name function name
 */
int			etrace_funcdisable(char *trace, char *name)
{
  trace_t		*ret_trace;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  table = etrace_get(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		  "Trace table not found", -1);

  ret_trace = hash_get(table, name);

  if (!ret_trace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Function not found", -1);

  if (ret_trace->enable != 0)
    trace_enabled_count--;

  ret_trace->enable = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Disable all functions of a trace 
 * @param trace trace name 
 * @see etrace_funcsettatus
 */
int			etrace_funcdisableall(char *trace)
{
  unsigned int			index;
  int			keynbr;
  char			**keys;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!trace)
    trace = ETRACE_TYPE_DEFAULT;

  etrace_init_trace();

  keys = hash_get_keys(&traces_table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  table = (hash_t *) hash_get(&traces_table, keys[index]);
	  
	  if (table)
	    etrace_funcsetstatus(table, 0);
	}

      hash_free_keys(keys);
    }  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Disable a function from a trace 
 * @param file target
 * @param name function name
 * @param optarg trace name (optional)
 */
int		traces_disable(elfshobj_t *file, char *name, char **optarg)
{
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!strcmp(name, "all"))
    {
      if (etrace_funcdisableall(optarg ? *optarg : NULL) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Disable all functions failed", -1);      
    }
  else
    {
      if (etrace_funcdisable(optarg ? *optarg : NULL, name) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Disable function failed", -1);
    }

  snprintf(buf, BUFSIZ - 1, "\t[*] Disabled function %s successfully from trace %s\n\n",
	   name, optarg && *optarg ? *optarg : ETRACE_TYPE_DEFAULT);
  aspectworld.profile(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
