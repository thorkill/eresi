


class A
{
public:
  char			A_a;
  short			A_b;
  int			A_c;
  unsigned long 	A_d;

  void			A_M_a() { 
    A_a = 'z';
    A_b = 42;
    A_c = 0xdeadbeef;
    A_d = 0xbadc0de;
  };

};



int	main()
{
  A	a;

  a.A_M_a();
}
