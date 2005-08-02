/*
** 
** infected.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Tue Oct  7 02:05:18 2003
** Updated : Tue Oct  7 02:35:00 2003
*/

void	hijacked(int arg)
{
  printf("arg %i hijacked\n", arg);
  subfunc(arg);
}
