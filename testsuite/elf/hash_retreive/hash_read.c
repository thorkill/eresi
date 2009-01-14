/*
** hash_read.c for testsuite in ERESI
** 
** Started on  Sun Mar 10 01:18:51 2002 jfv
** $Id: hash_read.c,v 1.3 2007-07-31 03:28:48 may Exp $
*/
#include	"libelfsh.h"

#if ERESI32
 #define		TROJANED_FILE	"./readme32"
#elif ERESI64
 #define		TROJANED_FILE	"./readme64"
#endif

#define		HASHED_SYMBOL	"printf"

int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  int		ret;
  char		*name;
  elfsh_Sym	*sym;

  file = elfsh_map_obj(TROJANED_FILE);
  if (!file)
    {
      elfsh_error();
      exit(-1);
    }

  printf("Value for %s retreived from .hash: %8p \n", HASHED_SYMBOL, 
	 elfsh_get_dynsymbol_by_hash(file, HASHED_SYMBOL));

  return (0);
}

