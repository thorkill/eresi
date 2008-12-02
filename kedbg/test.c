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

void plop(void)
{
  __asm__ __volatile__("movl $0xff, %eax");
}

int main(void)
{
  plop();
  foo(rand());
  puts("");

  return 0;
}
