#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  char	*str;

  str = malloc(10);
  if (str == NULL)
    goto err;
  strcpy(str, "test");
  printf("First_printf %s\n", str);
  fflush(stdout);
  puts("Second_puts");
  printf("Second_printf\n");
  fflush(stdout);
  return (0);
 err:
  printf("Malloc problem\n");
  return (-1);
}
