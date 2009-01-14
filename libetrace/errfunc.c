/**
 * @file errfunc.c
** @ingroup libetrace
**
** @brief This file contain one function for error collection.
**
**
** $Id$: errfunc.c,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"

/**
 * When an error occur in etrace. This function is used to show buggy function name.
 * @return function name
 */
char			*etrace_geterrfunc()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, last_parsed_function);
}


/* Start tracing after everything has been injected */
char		*etrace_start_tracing(elfshobj_t *file)
{
  char		*buggyfunc;

  profiler_error_reset();


  if (traces_run(file, NULL, 0) < 0)
    {
      buggyfunc = etrace_geterrfunc();
      profiler_error();
      return (buggyfunc);
    }
  return (NULL);
}
