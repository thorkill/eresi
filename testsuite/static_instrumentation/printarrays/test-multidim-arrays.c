/*
** Testsuite for reflection in ERESI
**
** $Id$
*/
#include <stdio.h>

int biarray[2][3] = {{1, 2, 3}, {4, 5, 6}};

int triarray[2][3][2] = {{{1, 2}, {3, 4}, {5, 6}}, {{7, 8}, {9, 10}, {11, 12}}};

int	main()
{
  printf("biarray = %p, triarray = %p \n", biarray, triarray);
  return (0);
}
