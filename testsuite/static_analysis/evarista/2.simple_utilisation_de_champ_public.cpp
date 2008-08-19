


class A
{
public:
  char			A_a;
  short			A_b;
  int			A_c;
  unsigned long 	A_d;
};



int	main()
{
  A	a;

  a.A_a = 'z';
  a.A_b = 42;
  a.A_c = 0xdeadbeef;
  a.A_d = 0xbadc0de;

  return (0);
}
