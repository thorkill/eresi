/*
** delete.c for e2dbg
**    
** Delete breakpoints
**
** Started on  Tue Aug 16 09:38:03 2005 mayhem                                                                                                                   
**
** $Id: step.c,v 1.2 2007-03-07 16:45:35 thor Exp $
**
*/
#include "libe2dbg.h"


/* Step command */
int		cmd_step()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

 retry:
  if (!e2dbgworld.curthread || !e2dbgworld.curthread->context)
    {
      if (e2dbgworld.sourcing)
	goto retry;
      else
	printf("wasnt sourcing ... existing \n");
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"You must be in a SIGTRAP handler to step", -1);
    }

  /* Also maintain a stepping flag in the world struct to keep 
     modelisation independance */
  if (e2dbgworld.curthread->step)
    {
      if (e2dbg_resetstep() < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Failed to disable stepping", -1);
      e2dbg_output("\n [*] Disabled stepping\n\n");
      e2dbgworld.curthread->step = 0;
    }
  else
    {
      if (e2dbg_setstep() < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Failed to enable stepping", -1);
      e2dbg_output("\n [*] Enabled stepping (now 'cont' to step) \n\n");
      e2dbgworld.curthread->step = 1;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

