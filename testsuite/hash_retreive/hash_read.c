/*
** unlink.c for unlinked
** 
** Started on  Sun Mar 10 01:18:51 2002 mayhem
** Last update Mon Mar 25 10:49:20 2002 mayhem
*/
#include	"libelfsh.h"


#define		TROJANED_FILE	"./a.out"
#define		OUTPUT_FILE	"./fake_aout"
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

  ret = elfsh_save_obj(file, OUTPUT_FILE);
  if (ret < 0)
    elfsh_error();

  printf("Relinking *%s* \n", ret ? "Error" : "OK");
  return (0);
}

