/*
** continue.c for e2dbg
**
** The continue command in e2dbg
**
*/
#include "elfsh.h"

/* (re)start a process in e2dbg */
void		e2dbg_start_proc()
{

  /* The debuggee now can continue */
  if (e2dbg_mutex_unlock(&e2dbgworld.dbgack) < 0)
    vm_output(" [E] Process already running\n\n");
#if __DEBUG_MUTEX__
  else
    printf(" [*] Debugger unlocked MUTEX-ACK and allow the debuggee to continue\n");
#endif
  
  /* We lock here so that we can wait for the debuggee to unlock dbgsyn */
  if (e2dbg_mutex_lock(&e2dbgworld.dbgsyn) < 0)
    printf(" [*] Debugger failed lock on DBGSYN\n");
#if __DEBUG_MUTEX__
  else
    printf(" [*] Debugger locked SYN, Debuggee can restart !\n");
#endif
  
  world.curjob->current->running = 1;
}

/* Start cmd */
int		cmd_start()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (world.curjob->current->running)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Process is already started", -1);
  if (!world.state.vm_quiet)
    vm_output(" [*] Starting process\n");
  e2dbg_start_proc();
  world.curjob->current->running = 1;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Cont makes start 3 times for unlocking mutexes around the signal handlers */
/* See the count in e2dbg/signal.c that makes e2dbg need this */
int	cmd_cont()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!world.curjob->current->running)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Current object not running", -1);

  if (!world.state.vm_quiet)
    vm_output(" [*] Continuing process\n");
  e2dbg_setregs();
  e2dbg_start_proc();
  if (!e2dbgworld.step)
    {
      e2dbg_start_proc();
      e2dbg_start_proc();
    }
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
