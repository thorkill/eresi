/*
** threads.c for e2dbg
**
** Hook the threads API for keeping thread debugger information accurate
**
** Last Update Thu July 06 19:37:26 2006 mm
*/
#include "elfsh.h"
#include <pthread.h>


/* Hooked start routine for all threads */
static void*		e2dbg_thread_start(void *param)
{
  e2dbgthread_t	*cur;
  void*		(*start)(void *param);
  char		key[15];
  
  snprintf(key, sizeof(key), "%u", (unsigned int) pthread_self());
  cur = hash_get(&e2dbgworld.threads, key);

#if __DEBUG_THREADS__
  printf(" [*] Starting thread ID %u \n", (unsigned int) pthread_self());
#endif

  cur->state = E2DBG_THREAD_STARTED;

  start = (void *) cur->entry;
  return ((*start)(param));
}


/* Stop all threads when a breakpoint happens */
void		e2dbg_thread_stopall()
{
  hashent_t     *actual;
  e2dbgthread_t	*cur;
  u_int         index;

#if __DEBUG_THREADS__
  printf(" [*] Stopping all threads \n");
#endif

  for (index = 0; index < e2dbgworld.threads.size; index++)
    for (actual = &e2dbgworld.threads.ent[index]; 
	 actual != NULL && actual->key != NULL; actual = actual->next)
      {
	cur = actual->data;
	if (cur->tid != vm_dbgid_get() && cur->tid != pthread_self())
	  {
	    pthread_kill(cur->tid, SIGSTOP);
#if __DEBUG_THREADS__
	    printf(" [*] Stopping thread ID %u \n", (unsigned int) cur->tid);
#endif
	  }
      }
}


/* Continue all threads after a breakpoint */
void		e2dbg_thread_contall()
{
  hashent_t     *actual;
  e2dbgthread_t	*cur;
  u_int         index;

#if __DEBUG_THREADS__
  printf(" [*] Continuing all threads \n");
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

  fct = (void *) e2dbgworld.pthreadcreate;
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

  fct = (void *) e2dbgworld.pthreadexit;

#if __DEBUG_THREADS__
  printf("PTHREAD_EXIT ! Calling back original at %08X \n", (elfsh_Addr) fct);
#endif

  (*fct)(retval);
  while (1)
    sleep(1);
}


/* Print all threads */
void		cmd_threads()
{
  hashent_t     *actual;
  e2dbgthread_t	*cur;
  u_int         index;
  char		logbuf[BUFSIZ];
  char		*time, *nl, *state, *entry;
  char		c;

#if __DEBUG_THREADS__
  printf(" [*] Continuing all threads \n");
#endif

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
	  case E2DBG_THREAD_INIT:
	    state = "INIT";
	    break;
	  case E2DBG_THREAD_STARTED:
	    state = "STARTED";
	    break;
	  case E2DBG_THREAD_FINISHED:
	    state = "FINISHED";
	    break;
	  default:
	    state = "UNKNOWN";
	  }
	entry = vm_resolve(world.curjob->current, (elfsh_Addr) cur->entry, NULL);
	snprintf(logbuf, BUFSIZ, " Thread ID %5u %c %8s --[ started on %s from %s \n", 
		 (unsigned int) cur->tid, c, state, time, entry);
	vm_output(logbuf);
      }

  if (index == 0)
    vm_output("\n [*] No threads in this process \n\n");
  else
    vm_output("\n");
}


