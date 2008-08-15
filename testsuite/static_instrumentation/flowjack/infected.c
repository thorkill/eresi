/*
** 
** infected.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Tue Oct  7 02:05:18 2003
** Updated : Tue Oct  7 02:35:00 2003
**
** $Id: infected.c,v 1.2 2007-03-07 16:45:36 thor Exp $
**
*/
#include <stdio.h>

void	hijacked(int arg)
{
  printf("arg %i hijacked\n", arg);
  subfunc(arg);
}
