/**
 * @file func_enable.c
** @ingroup libetrace
** 
** @brief trace table enable functions.
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: func_enable.c,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"


/**
 * Enable the function from the trace table 
 * @param trace trace name
 * @param name function name
 */
int			etrace_funcenable(char *trace, char *name)
{
  trace_t		*ret_trace;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!trace || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  table = etrace_get(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		  "Trace table not found", -1);

  ret_trace = hash_get(table, name);

  if (!ret_trace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Function not found", -1);

  if (ret_trace->enable != 1)
    trace_enabled_count++;

  ret_trace->enable = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Enable all functions of a trace 
 * @param trace trace name
 * @see etrace_funcsetstatus
 */
int			etrace_funcenableall(char *trace)
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
	    etrace_funcsetstatus(table, 1);
	}

      hash_free_keys(keys);
    }  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Enable a function from a trace 
 * @param file target
 * @param name function name
 * @param optarg trace name (optional)
 */
int		traces_enable(elfshobj_t *file, char *name, char **optarg)
{
  char		buf[BUFSIZ];  

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!strcmp(name, "all"))
    {
      if (etrace_funcenableall(optarg ? *optarg : NULL) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Enable all functions failed", -1);      
    }
  else
    {
      if (etrace_funcenable(optarg ? *optarg : NULL, name) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Enable function failed", -1);
    }

  snprintf(buf, BUFSIZ - 1, "\t[*] Enabled function %s successfully from trace %s\n\n",
	   name, optarg && *optarg ? *optarg : ETRACE_TYPE_DEFAULT);
  aspectworld.profile(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
