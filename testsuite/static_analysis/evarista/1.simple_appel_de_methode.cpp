/*
** appel simple d une methode.
*/

class C_A 
{
private:
  int a_;

public:
  C_A ()  { a_ = 0;};
  ~C_A () {};

public:
  void A_M_a() { a_++; };  
  void A_M_b() { a_--; };
};

int main()
{
  C_A ptr;

  ptr.A_M_a();
  ptr.A_M_b();
  return 0;
}
