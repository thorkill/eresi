/*
** main.c
** 
** Started on  Sun Mar 17 06:38:49 2002 mayhem
** Last update Sat Mar 23 11:00:10 2002 mayhem
*/
#include <stdio.h>
#include <unistd.h>

int main()
{
  char	*str;
  int	index;

  for (index = 0; index < 10; index++)
    {
      str = (char *) malloc(10);
      if (str == NULL)
	goto err;
      memcpy(str, "test_heap", 9);
      printf("%s [%u] \n", str, index);
    }

  puts("End !");
  return (0);
 err:
  puts("Malloc error");
  return (-1);
}
