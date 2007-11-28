/**
 * @file func_add.c
** func_add.c for libetrace
** 
** @brief add function to a trace table
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: func_add.c,v 1.1 2007-11-28 09:32:06 rival Exp $
**
*/
#include "libelfsh.h"
#include "libasm.h"
#include "libetrace.h"
#include "libetrace-extern.h"

/**
 * Add the function on the trace table 
 * @param trace trace name
 * @param name function name
 * @param newtrace allocated structure that describe the trace
 * @return newtrace pointer or NULL in case of failure
 */
elfshtraces_t 		*elfsh_traces_funcadd(char *trace, 
					      char *name,
					      elfshtraces_t *newtrace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !newtrace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);

  table = elfsh_traces_gettrace(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Trace table not found", NULL);

  if (hash_get(table, name) != NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Function already exist", NULL);

  trace_enabled_count++;
  hash_add(table, newtrace->funcname, (void *) newtrace);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newtrace);
}
