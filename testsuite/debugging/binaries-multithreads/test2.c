/*
**
** $Id$
**
*/
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

pthread_t thread1, thread2;
char *message1 = "T1";
char *message2 = "T2";

typedef void (*sighandler_t)(int);


int		sigtrap_handler(int signum)
{
  int		tid;

  tid = pthread_self();
  fprintf(stderr, "Received SIGTRAP in thread %u \n", tid);
  if (tid == thread1)
    pthread_kill(thread2, SIGUSR2);
  else
    pthread_kill(thread1, SIGUSR2);
  usleep(50000);
  fprintf(stderr, "<---- outside sigtrap handler for %u \n", tid);
  return (0);
}

int		sigusr2_handler(int signum)
{
  int		tid;

  tid = pthread_self();
  fprintf(stderr, "Received SIGUSR2 in thread %u \n", tid);
  usleep(50000);
  fprintf(stderr, "<---- outside sigusr2 handler for %u \n", tid);
  return (0);
}

void*	thread_func(void *str)
{
  while (1)
    {
      if (!strcmp((char *) str, message2))
	pthread_kill(thread1, SIGTRAP);
      else
	pthread_kill(thread2, SIGTRAP);
      fprintf(stderr, "%s\n", (char *) str);
      sleep(1);
    }
}

int main(int argc, char **argv)
{
  int  iret1, iret2;
  struct sigaction ac;

  bzero((void *) &ac, sizeof(ac));
  ac.sa_sigaction   = (void *) sigtrap_handler;
  sigaction(SIGTRAP, &ac, NULL);
  ac.sa_sigaction   = (void *) sigusr2_handler;
  sigaction(SIGUSR2, &ac, NULL);  

  iret1 = pthread_create( &thread1, NULL, thread_func, (void*) message1);
  iret2 = pthread_create( &thread2, NULL, thread_func, (void*) message2);
 retry:
  fprintf(stderr, "Now sleeping\n");
  sleep(30);
  goto retry;
  return (0);
}
