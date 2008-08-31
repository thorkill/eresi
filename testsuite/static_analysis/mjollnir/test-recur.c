#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int funk3(int b);

int funk1(int a){printf("%d\n",a); funk3(a + 2); return 0;}
int funk2(int a){printf("%d\n",a);funk1(a-1);return 1;}
int funk3(int b){printf("%d\n",b);funk2(b+1);funk1(666);return 0;}

int main(void)
{
 int x=0;
 char *z;

 z = NULL;

 if (z == NULL) {
  printf("z == null\n");
 }
 if (z == 0x0) {
  printf("z == 0x0\n");
 }
 
 if (x == 0) {
  printf("is zero\n");
 } else {
  printf("is not zero\n");
 }

	funk1(333);
	funk2(666);
	funk3(777);

  z = malloc(1024);
  memset(z,0x0,1024);
  read(1,z,1023);

 return 0;
}

