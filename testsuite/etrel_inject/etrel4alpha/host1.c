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
  puts("First_puts");
  free(str);
  printf("Second_printf %u\n", 42);
  puts("Second_puts");
  fflush(stdout);
  printf("End of main %s \n", "hehe");
  return (0);
 err:
  printf("Malloc problem %u\n", 42);
  return (-1);
}
