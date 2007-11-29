/*
** autotypes.c for kernsh
** 
** $Id: autotypes.c,v 1.4 2007-11-29 15:33:39 may Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"

/*  */
int		cmd_autotypes()
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  revm_callback_handler_remove();
  ret = kernsh_autotypes();
  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot autotypes", -1);
  revm_callback_handler_install(revm_get_prompt(), revm_line_handler);
  revm_column_update();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
