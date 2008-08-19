

void	val(int *c)
{
  *c = 43;
}



int	main()
{
  int	b;
  int	*a;

  b = 42;
  a = &b;
  val(a);

  return (*a);
}
