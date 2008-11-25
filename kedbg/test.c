#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
  usleep(2000000);
  foo(rand());
  puts("");

  return 0;
}
