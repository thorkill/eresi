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
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>

int	glvar_testreloc = 42;

int	glvar_testreloc_bss;
char	glvar_testreloc_bss2;
short	glvar_testreloc_bss3;


int	hook_func(char *str)
{
  int sd;

  printf("hook func %s !\n", str);
  
  return (old_legit_func(str));
}


int	puts_troj(char *str)
{
  int	local = 1;
  char  *str2;
  int   fd;
  char  name[16];
  void  *a;

  str2 = malloc(10);
  *str2 = 'Z';
  *(str2 + 1) = 0x00;

  glvar_testreloc_bss  = 43;
  glvar_testreloc_bss2 = 44;
  glvar_testreloc_bss3 = 45;

  memset(name, 0, 16);
  
  printf("Trojan injected ET_REL takes control now [%s:%s:%u:%u:%hhu:%hu:%u] \n",
	 str2, str,
	 glvar_testreloc,
	 glvar_testreloc_bss,
	 glvar_testreloc_bss2,
	 glvar_testreloc_bss3,
	 local);

  free(str2);

  gethostname(name, 15);
  printf("hostname : %s\n", name);

  printf("printf called from puts_troj [%s] \n", str);


  fd = open("/etc/services", 0, O_RDONLY);

  if (fd)
    {
      if ((a = mmap(0, 100, PROT_READ, MAP_PRIVATE, fd, 0)) == (void *) -1)
	{
	  perror("mmap");
	  close(fd);
	  printf("mmap failed : fd: %d\n", fd);
	  return (-1);
	}
      printf("-=-=-=-=-=- BEGIN /etc/services %d -=-=-=-=-=\n", fd);
      printf("host : %.60s\n", (char *) a);
      printf("-=-=-=-=-=- END   /etc/services %d -=-=-=-=-=\n", fd);
      printf("mmap succeed fd : %d\n", fd);
      close(fd);
    }      


  old_puts(str);
  fflush(stdout);
  return (0);
}

int	func2()
{
  return (42);
}
