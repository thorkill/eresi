/*
** threads.c for e2dbg
**
** Hook the threads API for keeping thread debugger information accurately up2date
**
** Last Update Thu July 06 19:37:26 2006 mm
*/
#include "elfsh.h"
#include <pthread.h>


/* Hooked start routine for all threads */
static void*		e2dbg_thread_start(void *param)
{
  e2dbgthread_t		*cur;
  void*			(*start)(void *param);
  char			key[15];
  pthread_attr_t	attr;
  int			ret;
#if __DEBUG_THREADS__
  char			logbuf[BUFSIZ];
#endif

  SETSIG;

  snprintf(key, sizeof(key), "%u", (unsigned int) pthread_self());
  cur = hash_get(&e2dbgworld.threads, key);

#if __DEBUG_THREADS__
  printf(" [*] Starting thread ID %u \n", (unsigned int) pthread_self());
#endif

  if (pthread_self() != vm_dbgid_get())
    e2dbgworld.threadnbr++;

  cur->state = E2DBG_THREAD_STARTED;

  pthread_getattr_np(cur->tid, &attr);
  ret = pthread_attr_getstack(&attr, (void **) &cur->stackaddr, (size_t *) &cur->stacksize);

#if __DEBUG_THREADS__
  if (ret)
    write(1, "Problem during pthread_attr_getstack\n", 37);
  else
    {
      ret = snprintf(logbuf, BUFSIZ, " [D] Thread ID %u has stack at addr %08X with size %u \n",
		     (unsigned int) cur->tid, cur->stackaddr, cur->stacksize);
      write(1, logbuf, ret);
    }
#endif
    
  /* Call the real entry point */
  start = (void *) cur->entry;
  return ((*start)(param));
}


/* Stop all threads when a breakpoint happens */
void		e2dbg_thread_stopall(int signum)
{
  hashent_t     *actual;
  e2dbgthread_t	*cur;
  u_int         index;
  u_int	called = rand() % 1000;

#if __DEBUG_THREADS__
  printf(" [*] Stopping all user threads \n");
#endif

#if __DEBUG_THREADS__
  printf("******** [%u] STOPALL (%u) threads [cur = %u [cnt = %u] dbg = %u ] ******* \n", 
	 called, e2dbgworld.threadnbr, (unsigned int) e2dbgworld.stoppedpid, 
	 e2dbgworld.curthread->count, vm_dbgid_get());
#endif

  for (index = 0; index < e2dbgworld.threads.size; index++)
    for (actual = &e2dbgworld.threads.ent[index]; 
	 actual != NULL && actual->key != NULL; actual = actual->next)
      {
	cur = actual->data;
	if (cur->tid != vm_dbgid_get() && cur->tid != e2dbgworld.stoppedpid)
	  {
#if __DEBUG_THREADS__
	    printf(" [%u] Sending %s to thread ID %u \n", called,
		   (signum == SIGSTOP ? "SIGSTOP" : "SIGUSR2"),
		   (unsigned int) cur->tid);
#endif
	    pthread_kill(cur->tid, signum);
	  }
      }

#if __DEBUG_THREADS__
  printf("--------- END OF STOPALL %u ------------ \n", called);
#endif
}


/* Continue all threads after a breakpoint */
void		e2dbg_thread_contall()
{
  hashent_t     *actual;
  e2dbgthread_t	*cur;
  u_int         index;

#if __DEBUG_THREADS__
  printf(" [*] Continuing all threads (curthread count = %u) \n",
	 e2dbgworld.curthread ? e2dbgworld.curthread->count : 42);
#endif

  for (index = 0; index < e2dbgworld.threads.size; index++)
    for (actual = &e2dbgworld.threads.ent[index]; 
	 actual != NULL && actual->key != NULL; actual = actual->next)
      {
	cur = actual->data;
	if (cur->tid != vm_dbgid_get())
	  {
#if __DEBUG_THREADS__
	    printf(" [*] Continuing thread ID %u \n", (unsigned int) cur->tid);
#endif
	    pthread_kill(cur->tid, SIGCONT);
	  }
      }
}



/* Hook for pthread_create */
int		pthread_create (pthread_t *__restrict __threadp,
				__const pthread_attr_t *__restrict __attr,
				void *(*__start_routine) (void *),
				void *__restrict __arg)
{
  e2dbgthread_t	*new;
  char		*key;
  int		ret;
  int		(*fct)(pthread_t *__restrict __threadp, 
		       __const pthread_attr_t *__restrict __attr,
		       void *__start_routine,
		       void *__restrict __arg);

  ELFSH_NOPROFILE_IN();

  fct = (void *) e2dbgworld.syms.pthreadcreate;
  ret = (*fct)(__threadp, __attr, e2dbg_thread_start, __arg);
  XALLOC(new, sizeof(e2dbgthread_t), -1);
  XALLOC(key, 15, -1);
  snprintf(key, 15, "%u", *(unsigned int *) __threadp);
  new->tid   = *(unsigned long *) __threadp;
  new->entry = __start_routine;
  time(&new->stime);
  hash_add(&e2dbgworld.threads, key, new);
  ELFSH_NOPROFILE_ROUT(ret);
}



/* Hook for pthread_exit */
void	pthread_exit(void *retval)
{
  void	(*fct)(void *retval);

  fct = (void *) e2dbgworld.syms.pthreadexit;

#if __DEBUG_THREADS__
  printf(" [D] PTHREAD_EXIT ! Calling back original at %08X \n", (elfsh_Addr) fct);
#endif

  e2dbgworld.threadnbr--;
  (*fct)(retval);
  while (1)
    sleep(1);
}


/* Hook for signal */
/*
__sighandler_t		signal(int signum, __sighandler_t fctptr)
{
  __sighandler_t	(*fct)(int signum, __sighandler_t fctptr);

  fct = (void *) e2dbgworld.signal;

#if __DEBUG_THREADS__
  printf(" [D] THREAD %u SIGNAL (SIGNUM %u) Calling back original at %08X \n", 
	 (unsigned int) pthread_self(), signum, (elfsh_Addr) fct);
#endif

  return ((*fct)(signum, fctptr));
}
*/



/* Print all threads */
int		cmd_threads()
{
  hashent_t     *actual;
  e2dbgthread_t	*cur;
  u_int         index;
  char		logbuf[BUFSIZ];
  char		*time, *nl, *state, *entry;
  char		c;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Change current working thread if passed a parameter */
  if (world.curjob->curcmd->param[0])
    {
      snprintf(logbuf, BUFSIZ, "%u", atoi(world.curjob->curcmd->param[0]));
      cur = hash_get(&e2dbgworld.threads, logbuf);
      if (!cur)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown thread", (-1));
      e2dbgworld.curthread = cur;
      snprintf(logbuf, BUFSIZ, " [*] Switched to thread %u \n\n", 
	       atoi(world.curjob->curcmd->param[0]));
      vm_output(logbuf);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Iterate on the threads hash table */
  for (index = 0; index < e2dbgworld.threads.size; index++)
    for (actual = &e2dbgworld.threads.ent[index]; 
	 actual != NULL && actual->key != NULL; actual = actual->next)
      {
	cur = actual->data;
	time = ctime(&cur->stime);
	nl = strchr(time, '\n');
	if (nl)
	  *nl = 0x00;
	c = (e2dbgworld.curthread == cur ? '*' : ' ');
	switch (cur->state)
	  {
	  case E2DBG_THREAD_INIT:	/* The thread has not started yet */
	    state = "INIT";
	    break;
	  case E2DBG_THREAD_STARTED:	/* The thread has started */
	    state = "STARTED";
	    break;
	  case E2DBG_THREAD_BREAKING:	/* The thread is processing a breakpoint */
	    state = "BREAKING";
	    break;
	  case E2DBG_THREAD_SIGUSR2:	/* The thread is processing a SIGUSR2 */
	    state = "SIGUSR2";
	    break;
	  case E2DBG_THREAD_BREAKUSR2:	/* The thread is beeing debugged */
	    state = "POSTUSR2";
	    break;
	  case E2DBG_THREAD_STOPPING:	/* The thread is beeing stopped */
	    state = "STOPPING";
	    break;
	  case E2DBG_THREAD_RUNNING:	/* The thread is running, been continued already */
	    state = "RUNNING";
	    break;
	  case E2DBG_THREAD_FINISHED:	/* The thread is finished */
	    state = "FINISHED";
	    break;
	  default:
	    state = "UNKNOWN";
	  }
	entry = vm_resolve(world.curjob->current, (elfsh_Addr) cur->entry, NULL);
	snprintf(logbuf, BUFSIZ, " Thread ID %5u %c %s %8s --[ started on %s from %s \n", 
		 (unsigned int) cur->tid, c, 
		 (cur->tid == vm_dbgid_get() ? "DBG" : "   "), 
		 state, time, entry);
	vm_output(logbuf);
      }

  if (index == 0)
    vm_output("\n [*] No threads in this process \n\n");
  else
    vm_output("\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
