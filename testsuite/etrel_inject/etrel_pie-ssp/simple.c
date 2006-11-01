/*
** rel.c for elfsh
**
** Contain all tests for ET_REL injection
**
** Started on  Mon Jun 23 06:56:04 2003 mayhem
** Last update Sun Aug 17 17:41:48 2003 mayhem
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


//int     glvar_testreloc = 42;

/*
int     glvar_testreloc_bss;
char    glvar_testreloc_bss2;
short   glvar_testreloc_bss3;
*/

int	fake_main(int argc, char **argv)
{
  /*
  glvar_testreloc_bss = 1;
  glvar_testreloc_bss2 = 'Z';
  glvar_testreloc_bss3 = 42;
  */

  old_printf("I am the main function, I have %d argc and my argv is %08X yupeelala \n", 
	     argc, argv); //, glvar_testreloc);

  write(1, "fake_main is calling write ! \n", 30);

  old_main(argc, argv);

  /*  old_printf("Last printf in hook_main [%08X:%08X:%c:%04X] \n", 
	     glvar_testreloc, glvar_testreloc_bss, 
	     glvar_testreloc_bss2, glvar_testreloc_bss3); */

  return (0);
}

/*
char*	fake_strcpy(char *dst, char *src)
{
  printf("The fucker wants to copy %s at address %08X \n", src, dst);
  return ((char *) old_strcpy(dst, src));
}
*/


/*
void	fake_stack_smash_handler(char func[], int damaged)
{
  static int i = 0;
  printf("calling printf from stack smashing handler %u\n", i++);
  if (i>3)
    old___stack_smash_handler(func, damaged);
  else
    printf("Same player play again [damaged = %08X] \n", damaged);
  printf("A second (%d) printf from the handler \n", 2);
}
*/


int fake_libc_start_main(void *one, void *two, void *three, void *four, void *five, void *six, void *seven)
{
  static int i = 0;

  old_printf("fake_libc_start_main \n");
  printf("start_main has been run %u \n", i++);
  return (old___libc_start_main(one, two, three, four, five, six, seven));
}
