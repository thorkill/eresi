// Test pointer-follow function calls detection.
#include <stdio.h>

f1() {printf("dupa\n"); }
f2() {printf("dupa2\n");}

main() {
  int (*fn)();
  int (*fn2)();
  fn=f1;
  fn2=f2;
  fn(fn2());
}
