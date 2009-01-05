#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int func1(char *str)
{
  puts(str);
  return 0;
}

// Potential double free in that function
int func2(char *str)
{
  puts(str);
  free(str);
  return 0;
}


// Potential Memory leak in that function
int func3(char **str)
{
  puts(*str);
  *str = (char *) malloc(BUFSIZ + 5);
  return 0;
}


int	mystrncmp(char *one, char *two, unsigned int n)
{
  int	idx;

  for (idx = 0; idx < n; idx++)
    if (one[idx] != two[idx])
      return (one[idx] - two[idx]);
  return (0);
}


// This example is interresting, it contains many kind of 
// incorrect heap manipulations or corruptions
int	main()
{
  char *buf;
  char *buf2;	
  char *swp;	
  int	ret;

  buf  = (char *) malloc(BUFSIZ);
  while (1)
    {
      buf2 = (char *) malloc(BUFSIZ);
      buf[0] = '\0';

      if ((ret = read(0, buf, BUFSIZ - 1)) < 0)
	exit(-1);
      buf[ret - 1] = '\0';
      if ((ret = read(0, buf2, BUFSIZ - 1)) < 0)
	exit(-1);
      if (ret == 0)
	exit(0);

      buf2[ret - 1] = '\0';

      if (ret > 5 && buf[5] == 's')
	{
	  swp = buf;
	  buf = buf2;
	  buf2 = swp;
	}

      if (!mystrncmp(buf, "func1", 5))
	func1(buf);
      else if (!mystrncmp(buf, "func2", 5))
	func2(buf);
      else if (!mystrncmp(buf, "func3", 5))
	func3(&buf);

      free(buf2);
    }
}
