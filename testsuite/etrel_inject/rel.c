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


int	glvar_testreloc = 42;

int	glvar_testreloc_bss;
char	glvar_testreloc_bss2;
short	glvar_testreloc_bss3;


int	puts_troj(char *str)
{
  int	local = 1;
  char  *str2;

  str2 = malloc(10);
  *str2 = 'Z';
  *(str2 + 1) = 0x00;

  glvar_testreloc_bss  = 43;
  glvar_testreloc_bss2 = 44;
  glvar_testreloc_bss3 = 45;
  
  printf("Trojan injected ET_REL takes control now [%s:%u:%u:%hhu:%hu:%u] \n",
	 str2,
	 glvar_testreloc,
	 glvar_testreloc_bss,
	 glvar_testreloc_bss2,
	 glvar_testreloc_bss3,
	 local);

  old_puts("Original puts called");
  fflush(stdout);
  return (0);
}

int	func2()
{
  return (42);
}
