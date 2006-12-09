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

  /* Do not allow to start a thread while another one is created or breaking */
  e2dbg_mutex_lock(&e2dbgworld.dbgbp);
  vm_output(" [*] BP MUTEX LOCKED [e2dbg_thread_start] \n");

  SETSIG;

  snprintf(key, sizeof(key), "%u", (unsigned int) pthread_self());
  cur = hash_get(&e2dbgworld.threads, key);

#if __DEBUG_THREADS__
  printf(" [*] Starting thread ID %u \n", (unsigned int) pthread_self());
#endif

  /* Register the thread as started */
  if (pthread_self() != vm_dbgid_get())
    e2dbgworld.threadnbr++;
  cur->state = E2DBG_THREAD_STARTED;

  /* Get stack information for this thread */
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
  vm_output(" [*] BP MUTEX UNLOCKED [e2dbg_thread_start] \n");
  e2dbg_mutex_unlock(&e2dbgworld.dbgbp);
  return ((*start)(param));
}


/* Only called when running a monothread program */
int		e2dbg_curthread_init(void *start)
{
  e2dbgthread_t	*new;
  char		*key;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(new, sizeof(e2dbgthread_t), -1);
  XALLOC(key, 15, -1);
  snprintf(key, 15, "%u", (unsigned int) getpid());
  new->tid   = (unsigned int) getpid();
  new->entry = (void *) e2dbgworld.real_main;
  time(&new->stime);
  hash_add(&e2dbgworld.threads, key, new);
  e2dbgworld.curthread = new;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
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

  /* Do not allow to create a thread while another ons is breaking */
  e2dbg_mutex_lock(&e2dbgworld.dbgbp);
  vm_output(" [*] BP MUTEX LOCKED [pthread_create] \n");

  fct = (void *) e2dbgworld.syms.pthreadcreate;
  ret = (*fct)(__threadp, __attr, e2dbg_thread_start, __arg);
  XALLOC(new, sizeof(e2dbgthread_t), -1);
  XALLOC(key, 15, -1);
  snprintf(key, 15, "%u", *(unsigned int *) __threadp);
  new->tid   = *(unsigned long *) __threadp;
  new->entry = __start_routine;
  time(&new->stime);
  hash_add(&e2dbgworld.threads, key, new);

  /* Unlock the thread mutex */
  vm_output(" [*] BP MUTEX UNLOCKED [pthread_create] \n");
  e2dbg_mutex_unlock(&e2dbgworld.dbgbp);

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




/* Print all threads state */
void		e2dbg_threads_print()
{
  e2dbgthread_t	*cur;
  u_int         index;
  char		logbuf[BUFSIZ];
  char		*time, *nl, *state, *entry;
  char		c;
  char		**keys;
  int		keynbr;

  keys = hash_get_keys(&e2dbgworld.threads, &keynbr);

  /* Iterate on the threads hash table */
  for (index = 0; index < keynbr; index++)
    {
	cur = hash_get(&e2dbgworld.threads, keys[index]);
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
	snprintf(logbuf, BUFSIZ, 
		 " Thread ID %10u %c %s %8s --[ started on %s from %s \n", 
		 (unsigned int) cur->tid, c, 
		 (cur->tid == vm_dbgid_get() ? "DBG" : "   "), 
		 state, time, entry);
	vm_output(logbuf);
      }

  if (index == 0)
    vm_output("\n [*] No threads in this process \n\n");
  else
    vm_output("\n");
}


/* Switch on another thread and and Print all threads */
int		cmd_threads()
{
  e2dbgthread_t	*cur; 
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Change current working thread if passed a parameter */
  if (world.curjob->curcmd->param[0])
    {

      //snprintf(logbuf, BUFSIZ, "%lu", strtoul(world.curjob->curcmd->param[0]));
      cur = hash_get(&e2dbgworld.threads, world.curjob->curcmd->param[0]);
      if (!cur)
	{
	  printf("Unknown thread -%s- \n", world.curjob->curcmd->param[0]);
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Unknown thread", (-1));
	}
      
      /* Save modified registers in curthread, and load the new registers set */
      e2dbg_setregs();
      e2dbgworld.curthread = cur;
      e2dbg_getregs();
      snprintf(logbuf, BUFSIZ, " [*] Switched to thread %s \n\n", 
	       world.curjob->curcmd->param[0]);
      vm_output(logbuf);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Print all threads */
  e2dbg_threads_print();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Stop all threads when a breakpoint happens */
void		e2dbg_thread_stopall(int signum)
{
  e2dbgthread_t	*cur;
  u_int         index;
  char		**keys;
  int		keynbr;

#if __DEBUG_THREADS__
  u_int	called = rand() % 1000;

  /*
  printf(" [*] Stopping all user threads \n");
  printf("******** [%u] STOPALL (%u) threads [cur = %u [cnt = %u] dbg = %u ] ******* \n", 
	 called, e2dbgworld.threadnbr, 
	 (unsigned int) e2dbgworld.stoppedthread->tid, 
	 e2dbgworld.curthread->count, vm_dbgid_get());
  */
#endif

  /* Iterate on the hash table of threads */
  keys = hash_get_keys(&e2dbgworld.threads, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      cur = hash_get(&e2dbgworld.threads, keys[index]);

      /* Do not get the context of initializing threads */
      if (cur->state == E2DBG_THREAD_INIT) // && signum == SIGUSR2)
	//signum = SIGSTOP;
	continue;
      
      /* Do not kill the debugger or the breaking thread */
      if (cur->tid != vm_dbgid_get() && cur->tid != getpid() &&
	  cur->tid != e2dbgworld.stoppedthread->tid)
	{
#if __DEBUG_THREADS__
	  fprintf(stderr, 
		  " [%u] Sending %s to thread ID %lu \n", called,
		  (signum == SIGSTOP ? "SIGSTOP" : "SIGUSR2"), cur->tid);
#endif
	  /* If we send a SIGUSR2, increment the number of contexts to get */
	  /* In case the thread is already stopped, send a CONT signal first */
	  if (signum == SIGUSR2)
	    {
	      e2dbgworld.threadgotnbr++;
	      if (cur->state == E2DBG_THREAD_STOPPING || 
		  cur->state == E2DBG_THREAD_BREAKUSR2)
		{
		  cur->state = E2DBG_THREAD_RUNNING;
		  fprintf(stderr, " pthread_kill(%lu, CONT) returned %d \n",
			  (unsigned long) cur->tid, 
			  pthread_kill(cur->tid, SIGCONT));
		}
	    }
	  
	  /* Else change the state of the thread */
	  else if (signum == SIGSTOP)
	    cur->state = E2DBG_THREAD_STOPPING;
	  
	  /* Send the signal */
	  //e2dbg_threads_print();
	  pthread_kill(cur->tid, signum);
	  //fprintf(stderr, " pthread_kill(%lu, STOP/USR2) returned %d \n",
	  //  (unsigned long) cur->tid, pthread_kill(cur->tid, signum));
	}
    }
  
#if __DEBUG_THREADS__
  //e2dbg_threads_print();
  //printf("--------- END OF STOPALL %u ------------ \n", called);
#endif
}





/* Continue all threads after a breakpoint */
void		e2dbg_thread_contall()
{
  hashent_t     *actual;
  e2dbgthread_t	*cur;
  u_int         index;

#if 1 // __DEBUG_THREADS__
  printf(" [*] Continuing all threads (curthread count = %u) \n",
	 e2dbgworld.curthread ? e2dbgworld.curthread->count : 42);
#endif

  /* Wake up the breaking thread in first */
  //pthread_kill(e2dbgworld.curthread->tid, SIGCONT);

#if __DEBUG_THREADS__
  printf(" [*] Continuing thread ID %u \n", (unsigned int) cur->tid);
#endif

  /* Wake up all other threads after */
  for (index = 0; index < e2dbgworld.threads.size; index++)
    for (actual = &e2dbgworld.threads.ent[index]; 
	 actual != NULL && actual->key != NULL; actual = actual->next)
      {
	cur = actual->data;
	if (cur->tid != getpid() && 
	    cur->tid != vm_dbgid_get()) // && cur->tid != e2dbgworld.curthread->tid)
	  {
#if __DEBUG_THREADS__
	    printf(" [*] Continuing thread ID %u \n", (unsigned int) cur->tid);
#endif
	    cur->state = E2DBG_THREAD_RUNNING;
	    //pthread_kill(cur->tid, SIGCONT);
	    pthread_kill(cur->tid, SIGCONT);
	  }
      }
}

