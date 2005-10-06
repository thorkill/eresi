


void *print_message_function_lib( void *ptr )
{
  char *message;
  int	idx;

  for (idx = 0; 1; idx++)
    {
      if (idx == 5)
	__asm__(".byte 0xCC");
      message = (char *) malloc(42);
      printf("%s: (pid = %u LIBTHREAD) message addr %08X \n", ptr, getpid(), message);
      //free(message);
      sleep(1);
    }
}
