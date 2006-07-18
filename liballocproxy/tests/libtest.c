#include <signal.h>
#include <stdio.h>
#include <pthread.h>

void *print_message_function_lib( void *ptr )
{
  char *message;
  int	idx;

  for (idx = 0; 1; idx++)
    {
      //if (idx == 5)
      //raise(SIGTRAP);
      message = (char *) malloc(42);
      printf("%s: (thread id = %u LIBTHREAD) new chunk addr %08X stack var addr %08X\n", 
	     ptr, pthread_self(), message, &message);
      sleep(1);
    }
}


void print_lib()
{
  while (1)
    {
      printf("coucou print_lib\n");
      sleep(1);
    }
}


void test_lib()
{

  pthread_t thread4;

  printf("begin test_lib\n");
  pthread_create( &thread4, NULL, (void *) print_lib, NULL);

  pthread_join( thread4, NULL);

  printf("end test_lib\n");
}
