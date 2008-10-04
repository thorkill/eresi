/*
** modtest.c for elfsh
** 
** Started on  Wed Feb 19 08:20:07 2003 jfv
** Last update Thu Mar  4 20:21:56 2004 jfv
**
** $Id: modtest.c,v 1.5 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"


/* ELFSH_DECMD(mod_newcmd); */


#define	CMD_MYTEST	"cmdtest"

int	mod_print()
{
  puts("\n [*] I do control the print command and you wont use it :P \n");
  return (0);
}

int	mod_newcmd()
{
  puts("\n [*] This is a new command, oh no ! \n");
  return (0);
}


void	elfsh_help()
{
  printf("Simple help handler for modtest ...\n");
}

void	elfsh_init()
{
  puts(" [*] ELFsh modtest init -OK- \n");
  revm_command_set(CMD_PRINT, mod_print, ERESI_ORIG, (unsigned int) ERESI_ORIG);
  revm_command_add(CMD_MYTEST, mod_newcmd, NULL, 0, "Simple example command");
}

void	elfsh_fini()
{
  puts(" [*] ELFsh modtest fini -OK- \n");
  revm_command_set(CMD_PRINT, cmd_print, ERESI_ORIG, (unsigned int) ERESI_ORIG);
  revm_command_del(CMD_MYTEST);
}

