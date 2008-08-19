
#include <iostream>

using namespace std;


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
  a.A_c = 42434445;
  a.A_d = 46474849;

  std::cout << a.A_a << " " << a.A_b << " " << a.A_c << " " << a.A_d << std::endl;

  std::cin >> a.A_a;
  std::cin.get();
  std::cin >> a.A_b;
  std::cin.get();
  std::cin >> a.A_c;
  std::cin.get();
  std::cin >> a.A_d;
  std::cin.get();

  std::cout << a.A_a << " " << a.A_b << " " << a.A_c << " " << a.A_d << std::endl;

  return (0);
}
