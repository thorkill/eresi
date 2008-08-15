/*
**
** $Id: host.c,v 1.2 2007-03-07 16:45:36 thor Exp $
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int	legit_func(char *str)
{
  printf("LEGIT FUNC\n");
  printf("legit func (%s) !\n", str);
  return (0);
}

int main()
{
  char	*str;
  char	buff[BUFSIZ];

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
  perror("malloc");
  printf("Malloc problem\n");
  return (-1);
}
