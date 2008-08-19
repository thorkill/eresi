

struct mystruct { int a; int b; };


int	proc(int *c)
{
  (*c)++;
}


int			main()
{
  struct mystruct	c;
  int			b = 42;

  proc(&b);
  proc(&c.a);
  proc(&c.b);
  return (c.b);
}
