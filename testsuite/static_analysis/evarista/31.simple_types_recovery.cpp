

int		main(int argc, char **argv)
{
  char		a = 1;
  short		b = 2;
  int		c = 3;
  long		d = 4;
  long long	e = 5;

  unsigned char		f = 6;
  unsigned short      	g = 7;
  unsigned int		h = 8;
  unsigned long		i = 9;
  unsigned long long	j = 10;

  a = f * argc;
  b = h + a;
  c = j % g;
  d = i ^ f;
  e = g;

  j = a - b;
  i = !j;
  h = g + 1;
  f = (c * g) + (d - e);

  return (f);
}
