/*
** symtab_extend.c for elfsh testsuite
** 
** Started on  Sun Mar 10 01:18:51 2002 mayhem
** Last update Mon Jun 30 11:13:50 2003 mayhem
*/
#include	"libelfsh.h"


#define		INPUT_FILE	"./a.out"
#define		OUTPUT_FILE	"./fake_aout"



int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  int		ret;
  Elf32_Sym	sym;
  char		name[256];
  u_int		idx;

  file = elfsh_map_obj(INPUT_FILE);
  if (!file)
    {
      elfsh_error();
      exit(-1);
    }


  /***********************************************************/
  /**************** TEST FOR SYMTAB + STRTAB INJECTION *******/
  /***********************************************************/

  printf("Writing string in .strtab and adding symbol in .symtab \n");

  for (idx = 0; idx < 50; idx++)
    {
      snprintf(name, sizeof(name), "%s_%u", "DH0WNZ", idx);
      sym = elfsh_create_symbol(0x42424242, 21, STT_FUNC, 0, 0, 0);
      ret = elfsh_insert_symbol(file->secthash[ELFSH_SECTION_SYMTAB], &sym, name);
      if (ret < 0)
	{
	  elfsh_error();
	  exit(-1);
	}
    }


  /***********************************************************/
  /************************ RELINKING ************************/
  /***********************************************************/

  ret = elfsh_save_obj(file, OUTPUT_FILE);
  if (ret < 0)
    elfsh_error();

  printf("Relinking *%s* \n", ret ? "Error" : "OK");
  return (0);
}

