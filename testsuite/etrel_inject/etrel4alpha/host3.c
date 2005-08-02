#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int	hook_func(char *str)
{
  printf("hook func %s !\n", str);
  return (0);
}

int	legit_func(char *str)
{
  printf("legit func (%s) !\n", str);
  return (0);
}

int main()
{
  char	*str;

  str = malloc(10);
  if (str == NULL)
    goto err;
  strcpy(str, "test");
  printf("First_printf %s\n", str);
  fflush(stdout);
  puts("First_puts");
  printf("Second_printf\n");
  puts("Second_puts");
  fflush(stdout);
  legit_func("test");
  return (0);
 err:
  printf("Malloc problem\n");
  return (-1);
}
