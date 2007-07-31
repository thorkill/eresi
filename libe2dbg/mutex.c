/*
** mutex.c for e2dbg
**
** The debugger needs mutexes inside signal handlers so we do not use pthread mutexes
** 
** Last update Wed Oct 11 18:22:59 2005 jfv
**
** $Id: mutex.c,v 1.4 2007-07-31 03:28:46 may Exp $
**
*/
#include "libe2dbg.h"



/* Mutex internal API */
int		e2dbg_mutex_init(elfshmutex_t *m)
{
  *m = E2DBG_MUTEX_UNLOCKED;
  return (0);
}

/* This is not really real-time for now */
int		e2dbg_mutex_lock(elfshmutex_t *m)
{
  while (*m == E2DBG_MUTEX_LOCKED)
    {
      if (e2dbgworld.exited)
	_exit(0);
      usleep(200);
    }
  *m = E2DBG_MUTEX_LOCKED;
  return (0);
}

/* Return an error if already unlocked */
int		e2dbg_mutex_unlock(elfshmutex_t *m)
{
  if (e2dbgworld.exited)
    _exit(0);
  if (*m == E2DBG_MUTEX_UNLOCKED)
    return (-1);
  *m = E2DBG_MUTEX_UNLOCKED;
  return (0);
}
