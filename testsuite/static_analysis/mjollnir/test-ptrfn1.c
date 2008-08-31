// Test pointer-follow function calls detection.
#include <stdio.h>


innafunkcja() { printf("dupa\n"); }

main() {
  int (*fn)();
  fn=innafunkcja;
  fn();
}
