


template < typename T >
class A
{
public:
  T	a;

  T	A_M_a(T b)
  {
    a = b;
  };

};


int	main()
{
  A<int>	a;
    
  a.A_M_a(42);
  return (0);
}
