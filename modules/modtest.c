/*
** modtest.c for elfsh
** 
** Started on  Wed Feb 19 08:20:07 2003 mayhem
** Last update Fri Mar  7 18:52:19 2003 mayhem
*/
#include "elfsh.h"

#define	CMD_TEST	"cmdtest"

int	my_cmd_print()
{
  puts("\n [*] I do control the print command and you wont use it :P \n");
  return (0);
}

int	my_new_cmd()
{
  puts("\n [*] This is a new command, oh no ! \n");
  return (0);
}

void	elfsh_init()
{
  puts(" [*] ELFsh modtest init -OK- \n");
  vm_setcmd(CMD_PRINT, my_cmd_print, ELFSH_ORIG, (u_int) ELFSH_ORIG);
  vm_addcmd(CMD_TEST, my_new_cmd, NULL, 0);
}

void	elfsh_fini()
{
  puts(" [*] ELFsh modtest fini -OK- \n");
  vm_setcmd(CMD_PRINT, cmd_print, ELFSH_ORIG, (u_int) ELFSH_ORIG);
  vm_delcmd(CMD_TEST);
}

