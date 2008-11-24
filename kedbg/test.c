#include <stdio.h>
#include <stdlib.h>

void bar(int num)
{
  fflush(stdout);
  printf("bar %u", num);
}

void foo(int num)
{
  printf("foo %u ", num);
  fflush(stdout);
  bar(num + 1);
}

int main(void)
{
  foo(rand());
  puts("");
  return 0;
}
