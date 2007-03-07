/*
** 
** recursivite.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Tue Oct  7 01:48:14 2003
** Updated : Tue Oct  7 19:43:29 2003
**
** $Id: recursivite.c,v 1.2 2007-03-07 16:45:36 thor Exp $
**
*/
#include <stdio.h>

void	subfunc(arg)
{
  if (arg)
    subfunc(arg - 1);
  printf("i = %i\n", arg)
;
}


int	main(int ac, char **av)
{
  subfunc(10);
}
