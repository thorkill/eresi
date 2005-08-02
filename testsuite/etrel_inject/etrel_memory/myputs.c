
int	myputs(char *str)
{
  printf("Hijacked puts !!! arg = %s \n", str);
  return (old_puts(str));
}
