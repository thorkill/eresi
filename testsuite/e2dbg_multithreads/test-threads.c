#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

void *print_message_function(void *ptr);

extern void *print_message_function_lib(void *);

typedef void (*sighandler_t)(int);


void	     sigtrap_handler(int num)
{
  printf("Received a SIGTRAP ! Waiting 5 secondes and continue ... \n");
  sleep(5);
}

void	     sigstop_handler(int num)
{
  printf("Received a SIGSTOP ! \n");
}

void	     sigusr2_handler(int num)
{
  printf("Received a SIGUSR2 ! \n");
}

main()
{
     pthread_t thread1, thread2, thread3;
     char *message1 = "Thread 1";
     char *message2 = "Thread 2";
     char *message3 = "Thread 3";
     int  iret1, iret2, iret3;
 
     /* Create independant threads each of which will execute function */
     //signal(SIGTRAP, (sighandler_t) sigtrap_handler);
     signal(SIGSTOP, (sighandler_t) sigstop_handler);
     signal(SIGUSR2, (sighandler_t) sigusr2_handler);

     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
     iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);
     iret3 = pthread_create( &thread3, NULL, print_message_function_lib, (void*) message3);

     test_lib();

     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */
     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL); 
     pthread_join( thread3, NULL); 

     printf("Thread 1 returns: %d\n",iret1);
     printf("Thread 2 returns: %d\n",iret2);
     printf("Thread 3 returns: %d\n",iret3);
     exit(0);
}




void	*print_message_function( void *ptr )
{
  char	*message;

  while (1)
  {
    message = malloc(42);
    printf("%s: (thread id = %u LIBTHREAD) message addr %08X message var: %08X\n", 
	   ptr, pthread_self(), message, &message);
    sleep(1);
  }
}
