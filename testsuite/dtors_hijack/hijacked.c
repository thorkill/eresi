


void	my_dtors() __attribute__ ((destructor));


void	new_dtors()
{
  printf("hijacked dtors ! \n");
}


void	my_dtors()
{
    printf("My dtors ! \n");
}


int	main()
{
  printf("Main ! Sleeping 5 secs ... \n");
  sleep(5);
}
