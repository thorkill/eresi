/**
* @file libetrace/func_status.c
** @ingroup libetrace
** 
** @brief add function to a trace table.
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id$
**
*/
#include "libetrace.h"


/**
 * Change the status of a whole trace 
 * @param table trace hash table
 * @param status new status (0 = disable, 1 = enable)
 * @see etrace_funcenableall
 * @see etrace_funcdisableall
 */
int			etrace_funcsetstatus(hash_t *table, int status)
{
  u_int			index;
  int			keynbr;
  char			**keys;
  trace_t		*ret_trace;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  etrace_init_trace();

  keys = hash_get_keys(table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  ret_trace = (trace_t *) hash_get(table, keys[index]);
	  
	  if (ret_trace)
	    {
	      if (ret_trace->enable != status)
		{
		  if (ret_trace->enable == 1)
		    trace_enabled_count--;
		  else
		    trace_enabled_count++;
		}
	      ret_trace->enable = status;
	    }
	}

      hash_free_keys(keys);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
