/**
 * @file func_enable.c
** func_enable.c for libetrace
** 
** @brief trace table enable functions
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: func_enable.c,v 1.1 2007-11-28 09:32:06 rival Exp $
**
*/
#include "libelfsh.h"
#include "libasm.h"
#include "libetrace.h"
#include "libetrace-extern.h"

/**
 * Enable the function from the trace table 
 * @param trace trace name
 * @param name function name
 */
int			elfsh_traces_funcenable(char *trace, char *name)
{
  elfshtraces_t		*ret_trace;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!trace || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  table = elfsh_traces_gettrace(trace);

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
 * @see elfsh_traces_funcsetstatus
 */
int			elfsh_traces_funcenableall(char *trace)
{
  u_int			index;
  int			keynbr;
  char			**keys;
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!trace)
    trace = ELFSH_TRACES_TYPE_DEFAULT;

  elfsh_traces_inittrace();

  keys = hash_get_keys(&traces_table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  table = (hash_t *) hash_get(&traces_table, keys[index]);
	  
	  if (table)
	    elfsh_traces_funcsetstatus(table, 1);
	}

      hash_free_keys(keys);
    }  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
