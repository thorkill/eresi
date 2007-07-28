/*
** mode.c for kernsh
** 
** $Id: mode.c,v 1.1 2007-07-28 15:02:23 pouik Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"

/* Switch mode : static/dynamic */
int		cmd_kmode()
{
  char          *param;
  char		buf[256];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  param = world.curjob->curcmd->param[0];

  if (param)
    {
      if (*param == 's' || *param == 'S')
	{
	  if (!libkernshworld.open_static)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Static kernel is not loaded !", -1);
	  
	  kernsh_set_static_mode();
	  revm_output("[*] kernsh is now in STATIC mode\n\n");
	}
      else if (*param == 'd' || *param == 'D')
	{
	  if (!libkernshworld.open)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Unable to switch to dynamic mode, please open the memory !", -1);
	  
	  kernsh_set_mem_mode();
	  revm_output("[*] kernsh is now in DYNAMIC mode\n\n");
	}
      
      else
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unknown mode for kernsh !", -1);
	}
    }
  else
    {
      if (kernsh_is_static_mode())
	param = "STATIC";
      
      else if (kernsh_is_mem_mode())
	param = "DYNAMIC";
      
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Unknown kernsh mode", -1);
      
      snprintf(buf, sizeof(buf), "[*] kernsh is in %s MODE\n\n", param);
      revm_output(buf);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  revm_endline();

#if defined(USE_READLN)
  rl_callback_handler_install(vm_get_prompt(), vm_ln_handler);
  readln_column_update();
#endif
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
