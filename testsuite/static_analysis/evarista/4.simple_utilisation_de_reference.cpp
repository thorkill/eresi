


int	main()
{
  int	b;
  int	&a = b;

  b = 42;
  a = 43;

  return (a);
}
