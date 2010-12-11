
#include <iostream>
#include <stdlib.h>

class	B
{
public:
  unsigned char	f;
  B() { f = 'Z'; };
};


class	A
{
private:
  int	c;
public:
  int	d;
  B	*t;

  A()     { c = 42; t = new B(); };
  void print() { std::cout << c << " " << d << " " << t->f << std::endl; };
};



int	main(int argc, char **argv)
{
  A	array[2][atoi(argv[1])][3];
  int	idx, idx2, idx3;

  for (idx = 0; idx < 2; idx++)
    {
      for (idx2 = 0; idx2 < atoi(argv[1]); idx2++)
	{
	  for (idx3 = 0; idx3 < 3; idx3++)
	    {
	      array[idx][idx2][idx3].d = idx + idx2 + idx3;
	      array[idx][idx2][idx3].print();
	    }
	}
    }
  return (-42);
}
