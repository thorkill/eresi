/*
** hijacked.c for elfsh
** 
** Started on  Sun Mar 10 02:41:10 2002 mayhem
** Last update Sun Mar 10 02:54:44 2002 mayhem
**
** $Id: hijacked.c,v 1.3 2007-05-16 23:36:37 may Exp $
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
