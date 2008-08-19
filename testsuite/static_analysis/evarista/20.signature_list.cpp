
#include <iostream>
#include <list>


int	main()
{
  std::list<int>	a;

  a.push_front(42);
  a.push_front(68);
  a.push_front(72);
  a.remove(68);

  return (0);
}
