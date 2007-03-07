/*
**
** $Id: hijacked.c,v 1.2 2007-03-07 16:45:36 thor Exp $
**
*/


void	new_sleep()
{
  printf("hijacked sleep ! \n");
}


int	main()
{
  printf("Main ! \n");
  sleep(5);
}
