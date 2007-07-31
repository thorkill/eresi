/*
** run.c for elfsh
** 
** Started on  Wed Feb 21 22:02:36 2001 jfv
**
** $Id: run.c,v 1.7 2007-07-31 03:28:46 may Exp $
**
*/
#include "elfsh.h"



/* Run the binary */
int		cmd_run()
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!world.curjob->curcmd->param[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  ret = execv(world.curjob->curcmd->param[0], 
	      world.curjob->curcmd->param);


#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot execute ELF binary", -1);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
