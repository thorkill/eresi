


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
  char			B_a;
  short			B_b;
  int			B_c;
  unsigned long 	B_d;

  void			B_M_a() { 
    B_a = 'a';
    B_b = 24;
    B_c = 0xdeadbeef;
    B_d = 0xbadc0de;
  };


};



int	main()
{
  B	b;

  b.B_M_a();
}
