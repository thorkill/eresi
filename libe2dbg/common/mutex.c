/**
* @file libe2dbg/common/mutex.c
** @brief Implement debugger mutexes inside signal handlers (no pthread mutexes)
** @ingroup common 
**
** $Id$
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
