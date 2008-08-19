


template <typename T>
class Somme
{
private:
  T a_;
  T b_;
public:
  Somme(T a, T b) { a_ = a; b_ = b; };
  
  T addition() { return a_ + b_; } ;
};

class Operation : public Somme<int>
{
public:
  Operation() : Somme<int>(2, 3) {} ;

  int resultat()
  {
    return addition();
  };
};

int main ()
{
  Operation op;
  int a;

  a = op.addition();
  a = op.resultat();
  return 0;
}
