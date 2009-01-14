/**
** @file source.c
** @ingroup libstderesi
** Started on  Fri May 16 15:18:35 2005 jfv
** $Id$: source.c,v 1.1 2007-11-29 14:01:56 may Exp $
*/
#include "libstderesi.h"

/** 
 * @brief Sourcing of ERESI scripts
 */
int		cmd_source()
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = revm_source(world.curjob->curcmd->param);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to source script", (-1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
