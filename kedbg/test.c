#include <stdio.h>

void bar(void)
{
  printf("bar");
  fflush(stdout);
}

void foo(void)
{
  printf("foo");
  fflush(stdout);
  bar();
}

int main(void)
{
  foo();
  puts("");
  return 0;
}
