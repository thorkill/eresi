

void	new_sleep()
{
  printf("hijacked sleep ! \n");
}


int	main()
{
  printf("Main ! \n");
  sleep(5);
}
