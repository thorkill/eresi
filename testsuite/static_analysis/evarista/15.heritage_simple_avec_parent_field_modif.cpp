


class A
{

protected:
  char			A_a;
  short			A_b;
  int			A_c;
  unsigned long 	A_d;

public:
  void			A_M_a() { 
    A_a = 'z';
    A_b = 42;
    A_c = 0xdeadbeef;
    A_d = 0xbadc0de;
  };

};

class B : public A
{

public:
  void			B_M_a() { 
    A_a = 'a';
    A_b = 24;
    A_c = 0xdeadbeef;
    A_d = 0xbadc0de;
  };


};



int	main()
{
  B	b;

  b.B_M_a();
}
