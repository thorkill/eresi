// Test pointer-follow function calls detection.

innafunkcja() {printf("dupa\n"); }

main() {
  int (*fn)();
  fn=innafunkcja;
  fn();
}
