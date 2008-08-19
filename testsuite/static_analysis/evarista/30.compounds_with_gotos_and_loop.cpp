
#include <stdio.h>


void	func1(int b)
{
  short	idx = 0;

 begin:
  idx++;
  do
    {
      switch (idx % 11)
	{
	case 1:
	  goto begin;
	  break;
	default:
	  idx++;
	}
    }
  while (idx < 16);
}



int	loop_on_prime(int init)
{
  char	a[20];
  int	i;

  for (i = 0; i < sizeof(a); i++, init++)
    {
      if (i % 2 && i % 5)
	init -= 0x30303030;
      else if (i % 3 && i % 7)
	func1(init);
      else
	goto end;
    }

 end:
  return (42);
}


int	main()
{
  printf("Testsuite 30 Triosigma project \n");
  return (loop_on_prime(0x41414141));
}
