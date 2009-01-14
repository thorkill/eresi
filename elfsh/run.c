/**
 * @file run.c
 * @ingroup elfsh
 * Started on  Wed Feb 21 22:02:36 2001 jfv
 *
 * $Id$: run.c,v 1.10 2008-02-16 12:31:34 thor Exp $
 *
 */
#include "elfsh.h"



/** 
 * Run the binary 
 */
int		cmd_run()
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!world.curjob->curcmd->param[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  revm_callback_handler_remove();
  ret = execv(world.curjob->curcmd->param[0], 
	      world.curjob->curcmd->param);
  revm_callback_handler_install(revm_get_prompt(), revm_line_handler);
  revm_column_update();
  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot execute ELF binary", -1);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
