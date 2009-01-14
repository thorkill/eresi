/**
** @file prefixes.c
** @ingroup libstderesi
** Started on  Fri Nov  2 15:21:56 2001 jfv
** Updated on  Thu Jan 04 11:26:11 2007 jfv
**
** $Id$
**
*/
#include "libstderesi.h"



/** 
 * Change the VM state as QUIET 
 */
int		cmd_quiet()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  world.state.revm_quiet = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Change the VM state as VERBOSE (default) 
 */
int		cmd_verb()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  world.state.revm_quiet = 0;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Turn on the FORCE flag 
 */
int             cmd_force()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  world.state.revm_force = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Precise a general weak bounded regex for all options 
 */
int		cmd_glregx()
{
  char		*str;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  str = strdup(world.curjob->curcmd->param[0]);
 
  if (regcomp(&world.state.revm_regx, str, REG_EXTENDED) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Failed to compute regex", -1);
  world.state.revm_use_regx = 1;
  if (world.state.revm_sregx)
    XFREE(__FILE__, __FUNCTION__, __LINE__,world.state.revm_sregx);
  world.state.revm_sregx = str;
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Changed global regex to %s \n\n", str);
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Read the sorting parameter 
 */
int		cmd_sort()
{
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->param[0][0] != ELFSH_SORT_BY_ADDR &&
      world.curjob->curcmd->param[0][0] != ELFSH_SORT_BY_SIZE)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid sorting type", -1);

  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Switched to %s sorting\n\n",
	       (world.curjob->curcmd->param[0][0] == ELFSH_SORT_BY_ADDR ? 
		"ADDR" : "SIZE"));
      revm_output(logbuf);
    }

  world.state.sort = world.curjob->curcmd->param[0];

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * Set the share flag 
 */
int		cmd_shared()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  world.state.revm_shared = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



