/*
** unlink.c for unlinked
** 
** Started on  Sun Mar 10 01:18:51 2002 mayhem
** Last update Sun Mar 10 02:56:20 2002 mayhem
*/
#include	"libelfsh.h"

#define		TROJANED_FILE	"./a.out"
#define		OUTPUT_FILE	"./fake_aout"


int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  Elf32_Sym	*new_dtors;
  int		ret;


  file = elfsh_map_obj(TROJANED_FILE);
  new_dtors = elfsh_get_symbol_by_name(file, "new_dtors");

  if (!file || !new_dtors)
    {
      elfsh_error();
      exit(-1);
    }


  /***********************************************************/
  /**************** TEST FOR DTORS HIJACKING *****************/
  /***********************************************************/


  //ret = elfsh_set_dtors_entry_by_index(file, 1, new_dtors->st_value);
  ret = elfsh_set_dtors_entry_by_name(file, "my_dtors", new_dtors->st_value);
  if (ret < 0)
    {
      elfsh_error();
      exit(-1);
    }


  /* RELINKING THE FILE */

  ret = elfsh_save_obj(file, OUTPUT_FILE);
  if (ret < 0)
    elfsh_error();

  printf("Relinking *%s* \n", ret ? "Error" : "OK");
  return (0);
}

