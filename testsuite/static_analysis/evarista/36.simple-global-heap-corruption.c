

char	*a;

int	main()
{
  a = malloc(42);
  a = malloc(30);
  if (a == 0)
    return (0);
  free(a);
  free(a);
  return (0);
}
