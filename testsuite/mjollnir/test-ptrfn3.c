#include <stdio.h>

int f1() {f4();}
int f2(){f4();}
int f3(){f4();}
int f4() {printf("dupa\n");}

int main(void) {
 int x;
 int a[3] = {(int)f1,(int)f2,(int)f3};
 int (*f) ();
 for(x=0;x<3;x++) {
  f=(int(*)())a[x];
  f();
 }
return 0;
}


