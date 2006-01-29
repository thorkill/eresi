/*
** mutex.c for e2dbg
**
** The debugger needs mutexes inside signal handlers so we do not use pthread mutexes
** 
** Last update Wed Oct 11 18:22:59 2005 mayhem
*/
#include "elfsh.h"



/* Mutex internal API */
int		e2dbg_mutex_init(elfshmutex_t *m)
{
  *m = ELFSH_MUTEX_UNLOCKED;
  return (0);
}

/* This is not really real-time for now */
int		e2dbg_mutex_lock(elfshmutex_t *m)
{
  while (*m == ELFSH_MUTEX_LOCKED)
    {
      if (e2dbgworld.exited)
	_exit(0);
      usleep(200);
    }
  *m = ELFSH_MUTEX_LOCKED;
  return (0);
}

/* Return an error if already unlocked */
int		e2dbg_mutex_unlock(elfshmutex_t *m)
{
  if (e2dbgworld.exited)
    _exit(0);
  if (*m == ELFSH_MUTEX_UNLOCKED)
    return (-1);
  *m = ELFSH_MUTEX_UNLOCKED;
  return (0);
}
