


int	proc(int c)
{
  c = 44;
}


int	main()
{
  int	b;
  int	&a = b;

  b = 42;
  proc(a);

  return (a);
}
