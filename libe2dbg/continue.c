/*
** continue.c for e2dbg
**
** The continue command in e2dbg
**
**
** $Id: continue.c,v 1.3 2007-03-07 16:45:35 thor Exp $
**
*/
#include "libe2dbg.h"

/* (re)start a process in e2dbg */
void		e2dbg_start_proc()
{
  if (e2dbgworld.curthread && e2dbgworld.curthread->count == 2)
    e2dbg_thread_contall();
  else if (!e2dbgworld.curthread)
    e2dbg_output(" [*] e2dbg_start_proc -NOT- doing CONTALL"
		 " because curthread = NULL\n");
  else
    printf(" [*] e2dbg_start_proc -NOT- doing CONTALL (count = %u)\n",
	   e2dbgworld.curthread->count);
  world.curjob->current->running = 1;
}


/* Start cmd */
int		cmd_start()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (world.curjob->current->running)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Process is already started", -1);
  if (!world.state.vm_quiet)
    e2dbg_output(" [*] Starting process\n");
  e2dbg_start_proc();
  world.curjob->current->running = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Cont makes start 3 times for unlocking mutexes around the signal handlers */
/* See the count in e2dbg/signal.c that makes e2dbg need this */
int	cmd_cont()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!world.curjob->current->running)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Current object not running", -1);

  if (!world.state.vm_quiet)
    e2dbg_output(" [*] Continuing process\n");

  /* Set back the current thread to the stopped thread */
  if (e2dbgworld.stoppedthread->tid != e2dbgworld.curthread->tid)
    {
      e2dbg_setregs();
      e2dbgworld.curthread = e2dbgworld.stoppedthread;
    }
  else
    e2dbg_setregs();

  /* Restart the debuggee */
  e2dbg_start_proc();

  /*
  if (!e2dbgworld.curthread->step)
    {
      e2dbg_start_proc();
      e2dbg_start_proc();
    }
  */

  e2dbg_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
