/**
* @file libe2dbg/user/continue.c
** @ingroup user
*/
/**
* @file libe2dbg/user/continue.c
 *
 * The continue command in e2dbg
 *
 *
 * $Id$
 *
 */
#include "libe2dbg.h"

/**
 * (re)start a process in e2dbg 
 */
void		e2dbg_start_proc()
{
  if (e2dbgworld.curthread && 
      e2dbgworld.curthread->count == E2DBG_BREAK_FINISHED)
    e2dbg_thread_contall();
  
#if __DEBUG_THREADS_
  else if (!e2dbgworld.curthread)
    e2dbg_output(" [*] e2dbg_start_proc -NOT- doing CONTALL"
		 " because curthread = NULL\n");
  else
    printf(" [*] e2dbg_start_proc -NOT- doing CONTALL (count = %u)\n",
	   e2dbgworld.curthread->count);
#endif

  world.curjob->curfile->running = 1;
}


/** 
 * Start cmd 
 * @return
 */
int		cmd_start()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (world.curjob->curfile->running)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Process is already started", -1);
  if (!world.state.revm_quiet)
    e2dbg_output(" [*] Starting process\n");
  e2dbg_start_proc();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, REVM_SCRIPT_CONTINUE);
}


/**
 * Cont makes start 3 times for unlocking mutexes around the signal handlers.
 * See the count in e2dbg/signal.c that makes e2dbg need this 
 * @return
 */
int	        cmd_cont()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!world.curjob->curfile->running)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Current object not running", -1);

  if (!world.state.revm_quiet)
    e2dbg_output(" [*] Continuing process\n");

  if (e2dbgworld.curthread->step)
    {
      if (e2dbg_resetstep() < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Failed to disable stepping", -1);
      e2dbgworld.curthread->step = 0;
      e2dbgworld.curthread->count = E2DBG_BREAK_FINISHED;
    }

  /* Set back the current thread to the stopped thread */
  e2dbg_setregs();
  if (e2dbgworld.stoppedthread->tid != e2dbgworld.curthread->tid)
    e2dbgworld.curthread = e2dbgworld.stoppedthread;

  /* Restart the debuggee */
  e2dbg_start_proc();
  e2dbg_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, REVM_SCRIPT_CONTINUE);
}
