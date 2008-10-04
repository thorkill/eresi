/**
 * @file func_remove.c
** @ingroup libetrace
** 
** @brief trace table remove functions.
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: func_remove.c,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"


/**
 * Delete the function from the trace table 
 * @param trace trace name
 * @param name function name
 */
int			etrace_funcrm(char *trace, char *name)
{
  trace_t		*ret_trace;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  table = etrace_get(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Trace table not found", -1);

  ret_trace = hash_get(table, name);

  if (ret_trace)
    {
      hash_del(table, name);
      XFREE(__FILE__, __FUNCTION__, __LINE__, ret_trace);
      trace_enabled_count--;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Delete all functions of a trace 
 * @param trace trace name
 */
int			etrace_funcrmall(char *trace)
{
  char			**keys;
  unsigned int			index;
  int			keynbr;
  trace_t		*ret_trace;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  table = etrace_get(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Trace table not found", -1);

  keys = hash_get_keys(table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  ret_trace = (trace_t *) hash_get(table, keys[index]);
	  hash_del(table, keys[index]);

	  if (ret_trace)
	    XFREE(__FILE__, __FUNCTION__, __LINE__, ret_trace);
	}

      hash_free_keys(keys);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


 
/** 
 * Delete a function from a trace 
 * @param file target
 * @param name function name
 * @param optarg trace name (optional)
 */
int		traces_rm(elfshobj_t *file, char *name, char **optarg)
{
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !name[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (etrace_funcrm(optarg ? *optarg : NULL, name) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Delete function failed", -1);

  snprintf(buf, BUFSIZ - 1, "\t[*] Deleted function %s successfully from trace %s\n\n",
	   name, optarg && *optarg ? *optarg : ETRACE_TYPE_DEFAULT);
  aspectworld.profile(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
