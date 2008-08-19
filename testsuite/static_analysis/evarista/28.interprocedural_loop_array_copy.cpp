
#include <stdio.h>


void	func1(int *a, int b)
{
  *a = b;
}



int	func2(int init)
{
  int	a[20];
  int	i;

  for (i = 0; i < sizeof(a); i++, init++)
    {
      if (i % 2)
	init -= 0x30303030;
      else if (i % 3)
	func1(a + i, init);
    }
  return (42);
}


int	main()
{
  printf("Testsuite 28 Triosigma project \n");
  return (func2(0x41414141));
}
