#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int	new_puts(char *str)
{
  printf("hijacked puts !\n");
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
  return (0);
 err:
  printf("Malloc problem\n");
  return (-1);
}
