/*
** sht_removing.c for testsuite in ERESI
** 
** Started on  Sun Mar 17 05:57:23 2002 jfv
** $Id: sht_removing.c,v 1.3 2007-07-31 03:28:48 may Exp $
*/
#include "libelfsh.h"

#if ERESI32
 #define		TROJANED_FILE	"./shtstripme32"
 #define		OUTPUT_FILE	"./shtstripped32"
#elif ERESI64
 #define		TROJANED_FILE	"./shtstripme64"
 #define		OUTPUT_FILE	"./shtstripped64"
#endif

/* Here we map, remove sht, and save the binary */
int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  elfsh_Shdr	hdr;
  elfsh_Shdr	unmapped_hdr;
  elfshsect_t	*new;
  elfshsect_t	*unmapped_new;
  int		ret;

  file = elfsh_map_obj(TROJANED_FILE);
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







