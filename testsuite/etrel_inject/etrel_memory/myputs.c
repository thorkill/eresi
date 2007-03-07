/*
**
** $Id: myputs.c,v 1.2 2007-03-07 16:45:36 thor Exp $
**
*/

int	myputs(char *str)
{
  printf("Hijacked puts !!! arg = %s \n", str);
  return (old_puts(str));
}
