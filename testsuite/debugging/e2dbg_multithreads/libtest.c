/*
**
** $Id: libtest.c,v 1.3 2007-03-17 13:05:31 may Exp $
**
*/
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void	*print_message_function_lib( void *ptr )
{
  char *message;
  int	idx;

  for (idx = 0; 1; idx++)
    {
      message = (char *) malloc(42);
      fprintf(stderr, "%s: (thread id = %u LIBTHREAD) new chunk addr %08X stack var addr %08X\n", 
	     ptr, pthread_self(), message, &message);
      sleep(1);
    }
}


void print_lib()
{
  while (1)
    {
      fprintf(stderr, "coucou print_lib (thread id = %u) no malloc here \n",
	      pthread_self());
      sleep(1);
    }
}


void test_lib()
{
  pthread_t thread4;

  fprintf(stderr, "begin test_lib\n");
  pthread_create( &thread4, NULL, (void *) print_lib, NULL);
  pthread_join( thread4, NULL);
  fprintf(stderr, "end test_lib\n");
}
