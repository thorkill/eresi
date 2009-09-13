/**
* @file libetrace/delete.c
** @ingroup libetrace
** 
** @brief trace deletion function
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id$
**
*/
#include "libelfsh.h"
#include "libasm.h"
#include "libetrace.h"

/**
 * Delete a trace 
 * @param trace trace name
 */
int			etrace_deletetrace(char *trace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  etrace_funcrmall(trace);

  table = etrace_get(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't found this trace", -1);

  hash_destroy(table);
  hash_del(&traces_table, trace);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
