/*
** error.c
** 
** Started on  Thu Nov  8 02:08:28 2001 mayhem
** Last update Sat Apr 12 10:01:42 2003 mayhem
*/
#include "libelfsh.h"


char	*elfsh_error_msg = NULL;


void	elfsh_error()
{
  fprintf(stderr, "%s\n", 
	  (elfsh_error_msg != NULL ? elfsh_error_msg : ""));
}

