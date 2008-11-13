#include <stdio.h>

void bar(void)
{
  printf("bar");
}

void foo(void)
{
  printf("foo");
  bar();
}

int main(void)
{
  foo();
  puts("");
  return 0;
}
