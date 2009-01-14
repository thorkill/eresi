/*
** @file trace_flush.c
**
** Libetrace code for the ERESI project.
**
** Started on  Sun Jun 22 12:10:12 2007 jfv
** $Id$
**
*/
#include "libetrace.h"


/**
 * @brief Delete a trace.
 * @param file target
 * @param name trace name
 * @param optarg not use
 */
int		traces_delete(elfshobj_t *file, char *name, char **optarg)
{
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !name[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (etrace_deletetrace(name) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Delete trace failed", -1);

  snprintf(buf, BUFSIZ - 1, "\t[*] Deleted trace %s successfully\n\n", name);
  aspectworld.profile(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Flush a whole trace 
 * @param name trace name
 * @param optarg not use
 */
int		traces_flush(elfshobj_t *file, char *name, char **optarg)
{
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !name[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (etrace_funcrmall(name) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Flush trace failed", -1);

  snprintf(buf, BUFSIZ - 1, "\t[*] Flushed trace %s successfully\n\n", name);
  aspectworld.profile(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

