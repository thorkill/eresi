/*
**
** $Id$: host.c,v 1.2 2007-03-07 16:45:36 thor Exp $
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int	legit_func(char *str)
{
  puts("legit func !");
  return (0);
}

int main()
{
  char	*str;
  char	buff[BUFSIZ];

  read(0, buff, BUFSIZ-1);

  puts("First_puts");

  puts("Second_puts");

  fflush(stdout);

  legit_func("test");
 
  return (0);
}
