/*
** sht_extend.c
** 
** Started on  Sun Mar 17 05:57:23 2002 mayhem
** Last update Thu Jun  6 04:46:18 2002 mayhem
*/
#include "libelfsh.h"

#define         STRIPPED_FILE   "./a.out"
#define         OUTPUT_FILE     "./fake_aout"




/* Here we map, remove sht, and save the binary */
int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  elfsh_Shdr	hdr;
  elfsh_Shdr	unmapped_hdr;
  elfshsect_t	*new;
  elfshsect_t	*unmapped_new;
  int		ret;

  file = elfsh_map_obj(STRIPPED_FILE);
  if (!file)
    {
      elfsh_error();
      exit(-1);
    }

  
  printf("=> Removing the Section header table \n");
  elfsh_remove_sht(file);


  /***********************************************************/
  /************************ RELINKING ************************/
  /***********************************************************/


  ret = elfsh_save_obj(file, OUTPUT_FILE);
  if (ret < 0)
    elfsh_error();

  printf("Relinking *%s* \n", ret ? "Error" : "OK");
  return (0);

}







