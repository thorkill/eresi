/*
**
** $Id$
**
*/
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

pthread_t thread1, thread2;
char *message1 = "T1";
char *message2 = "T2";

typedef void (*sighandler_t)(int);


int		sigtrap_handler(int signum)
{
  fprintf(stderr, "Received SIGTRAP in thread %u ! \n", pthread_self());
  pthread_kill(thread2, SIGSTOP);
  sleep(2);
  fprintf(stderr, "<---- outside sigtrap handler \n");
  return (0);
}

int		sigstop_handler(int signum)
{
  fprintf(stderr, "Received SIGSTOP in thread %u ! \n", pthread_self());
  fprintf(stderr, "<---- outside sigstop handler \n");
  return (0);
}

void*	thread_func(void *str)
{
  while (1)
    {
      if (!strcmp((char *) str, message2))
	pthread_kill(thread1, SIGTRAP);
      fprintf(stderr, "%s\n", (char *) str);
      sleep(1);
    }
}

int main(int argc, char **argv)
{
  int  iret1, iret2;

  signal(SIGTRAP, (sighandler_t) sigtrap_handler);
  signal(SIGSTOP, (sighandler_t) sigstop_handler);  
  iret1 = pthread_create( &thread1, NULL, thread_func, (void*) message1);
  iret2 = pthread_create( &thread2, NULL, thread_func, (void*) message2);
 retry:
  fprintf(stderr, "Now sleeping\n");
  sleep(30);
  goto retry;
  return (0);
}
