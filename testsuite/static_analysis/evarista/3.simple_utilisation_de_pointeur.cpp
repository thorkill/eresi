


int	main()
{
  int	b;
  int	*a;

  b = 42;
  a = &b;
  *a = 43;

  return (*a);
}
