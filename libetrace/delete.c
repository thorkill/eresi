/**
 * @file delete.c
** delete.c for libetrace
** 
** @brief trace deletion function
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: delete.c,v 1.1 2007-11-28 09:32:06 rival Exp $
**
*/
#include "libelfsh.h"
#include "libasm.h"
#include "libetrace.h"

/**
 * Delete a trace 
 * @param trace trace name
 */
int			elfsh_traces_deletetrace(char *trace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  elfsh_traces_funcrmall(trace);

  table = elfsh_traces_gettrace(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't found this trace", -1);

  hash_destroy(table);
  hash_del(&traces_table, trace);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
