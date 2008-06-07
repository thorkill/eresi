/*
**
** $Id: hijacked.c,v 1.3 2007-05-16 23:36:37 may Exp $
**
*/
#include <stdio.h>

void	new_sleep()
{
  printf("hijacked sleep ! \n");
}


int	main()
{
  printf("Main ! \n");
  sleep(5);
}
