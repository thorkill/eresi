/*
 *
 * cc -o vuln vun.c
 * ./vuln 1234567890123456
 *
 */

#include <stdio.h>

#ifdef __hppa
unsigned long __guard = 0UL;
#else
extern unsigned long __guard;
#endif

#ifdef VULN_HEAP
#define BUF_TYPE *buf
#else
#define BUF_TYPE buf
#endif

int main(int argc, char **argv, char **envp, char **auxv) {
#ifdef VULN_HEAP
	char *buf = (char *) malloc(10);
#else
	char buf[10] = "";
#endif
	char buf1[] = "01234567890123456789";
	printf("%p main(argc=%p, argv=%p, envp=%p, auxv=%p) __guard=%p\n", 
		&main, &argc, &argv, &envp, &auxv, &__guard);
#ifdef __SSP__
fputs("ssp", stdout);
#endif

#ifdef __SSP_ALL__
fputs("-all ", stdout);
#endif

	fflush(stdout);
#ifdef VULN_HEAP
	printf("(Heap) ");
#else
	printf("(Stack) ");
#endif
	if (argc > 1) {
		if (strcmp(argv[1], "-a") == 0) {
			while(1) {
				printf("Appending [%d] to a buffer thats [%d] bytes long with a max buffer size of [10]\n",
					1, strlen(buf)+1);
				fflush(stdout);
				strcat(buf, "A");
			}
		}
		printf("Copying [%d] of data into [%d] of space\n",
		strlen(argv[1]), 10);
		strcpy(buf, argv[1]);
	} else {
		printf("Triggering an overflow by copying [%d] of data into [%d] of space\n",
			strlen(buf1), 10);
		strcpy(buf, buf1);
	}
	return 0;
}
