#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>

int testcrypt()
{
  crypt("password", "salt");
  return 5;
}

int firstfunc(int num, char *value)
{
  char *monptr;
  monptr = (value + 3 % 1);
  testcrypt();
  return 1;
}

int main(int argc, char **argv)
{
  unsigned int index;
  for (index = 0; index < argc; index++)
    {
      printf("arg[%d] %s\n", index, argv[index]);
    }
  char path[256];
  getcwd(path, 255);
  printf("BEFORE ! %s\n", path);
  firstfunc(3, "this is the text");
  return 0;
}
