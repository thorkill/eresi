/*
** 
** target.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Tue Oct  7 01:45:28 2003
** Updated : Tue Oct  7 02:06:29 2003
*/
#include <stdio.h>

void	subfunc(int arg)
{
  printf("i = %i\n", arg);
}

int	func1(void)
{
  subfunc(1);
}

int	func2(void)
{

  subfunc(2);
}

int	func3(void)
{
  subfunc(3);
  return (3);
}

int	main()
{
  func1();
  func2();
  func3();
  return (0);
}
