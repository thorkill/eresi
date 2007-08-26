/*
** autotypes.c for kernsh
** 
** $Id: autotypes.c,v 1.2 2007-08-26 18:07:09 pouik Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"

/*  */
int		cmd_autotypes()
{
  int		ret;
  //char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  ret = kernsh_autotypes();

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot autotypes", -1);

#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
