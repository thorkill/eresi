/*
** delete.c for e2dbg
**    
** Delete breakpoints
**
** Started on  Tue Aug 16 09:38:03 2005 jfv
**
** $Id$: step.c,v 1.1 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libe2dbg.h"


/* Perform stepping */
int		e2dbg_step()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Checking stepping flag in the thread structure */
  if (e2dbgworld.curthread->step)
    {
      if (e2dbg_resetstep() < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Failed to disable stepping", -1);
      e2dbg_output("\n [*] Disabled stepping\n\n");
      e2dbgworld.curthread->step = 0;
      e2dbgworld.curthread->count = E2DBG_BREAK_NONE;
      e2dbgworld.curthread->was_step = 1;
    }

  /* Disable stepping and continue */
  else
    {
      if (e2dbg_setstep() < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Failed to enable stepping", -1);
      e2dbg_output("\n [*] Enabled stepping \n\n");
      e2dbgworld.curthread->step = 1;
      e2dbg_setregs();
      if (e2dbgworld.stoppedthread->tid != e2dbgworld.curthread->tid)
	e2dbgworld.curthread = e2dbgworld.stoppedthread;
      e2dbg_thread_contall();
      world.curjob->curfile->running = 1;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, REVM_SCRIPT_CONTINUE);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Step command */
int		cmd_step()
{
  int		ret;

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

  ret = e2dbg_step();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/* Step-trace command */
int		cmd_itrace()
{
  int		ret;

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

  /* Enable or disable trace flag */
  if (e2dbgworld.curthread->trace)
    e2dbgworld.curthread->trace = 0;
  else
    e2dbgworld.curthread->trace = 1;
  ret = e2dbg_step();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
