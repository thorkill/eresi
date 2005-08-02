#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	legit_func(char *str)
{
  printf("LEGIT FUNC\n");
  printf("legit func (%s) !\n", str);
  return (0);
}

int main(int argc, char **argv)
{
  char	*str;
  char	buff[BUFSIZ];



  printf ("[host] main argc %d\n", argc);
  

  printf("[host] argv[0] is : %s\n", argv[0]);

  if (argc == 2)
    printf("[host] argv[1] is : %s\n", argv[1]);


  

  read(0, buff, BUFSIZ-1);

  str = malloc(10);
  if (str == NULL)
    goto err;
  strcpy(str, "test");
  printf("First_printf %s\n", str);
  fflush(stdout);
  puts("First_puts");
  printf("Second_printf %s\n", str);

  free(str);

  puts("Second_puts");

  fflush(stdout);
  legit_func("test");
  return (0);
 err:
  printf("Malloc problem\n");
  return (-1);
}
