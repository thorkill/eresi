


void *print_message_function_lib( void *ptr )
{
  char *message;

  while (1)
    {
      message = malloc(42);
      printf("%s: (pid = %u LIBTHREAD) message addr %08X \n", ptr, getpid(), message);
      //free(message);
      sleep(1);
    }
}
