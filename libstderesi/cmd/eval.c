/**
** @file eval.c
** @ingroup libstderesi
**
** Started on Thu Nov 13 13:03:14 2008 jfv
** $Id$
*/
#include "libstderesi.h"

int		cmd_eval()
{
  revmexpr_t	*res;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  res = revm_compute(world.curjob->curcmd->param[0]);
  if (!res)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to evaluate expression", -1);
  revm_expr_print(res, 0);
  revm_output("\n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
