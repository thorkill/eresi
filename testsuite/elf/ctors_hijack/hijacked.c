/*
** hijacked.c for elfsh
** 
** Started on  Sun Mar 10 02:41:10 2002 jfv
** Last update Sun Mar 10 02:54:44 2002 jfv
**
** $Id$: hijacked.c,v 1.4 2007-07-31 03:28:48 may Exp $
**
*/
#include <stdio.h>


void	my_ctors() __attribute__ ((constructor));


void	new_ctors()
{
  printf("hijacked ctors ! \n");
}


void	my_ctors()
{
  printf("My ctors ! \n");

}

int	main()
{
  printf("Main ! \n");
}
