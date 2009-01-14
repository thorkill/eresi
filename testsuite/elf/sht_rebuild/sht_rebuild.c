/*
** sht_rebuild.c for testsuite in ERESI
** 
** Started on  Sun Mar 17 05:57:23 2002 jfv
** $Id$
*/
#include "libelfsh.h"
#include <stdio.h>

#if ERESI32
 #define		TROJANED_FILE	"./shtstripped32"
 #define		OUTPUT_FILE	"./shtrebuilt32"
#elif ERESI64
 #define		TROJANED_FILE	"./shtstripped64"
 #define		OUTPUT_FILE	"./shtrebuilt64"
#endif

/* We just map and save the object since ELFsh automatically rebuid SHT */
int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  elfsh_Shdr	hdr;
  elfsh_Shdr	unmapped_hdr;
  elfshsect_t	*new;
  elfshsect_t	*unmapped_new;
  int		ret;

  /* map the object */
  file = elfsh_map_obj(TROJANED_FILE);
  if (!file)
    {
      elfsh_error();
      exit(-1);
    }

  /* Get (and reconstruct if unexistant) SHT and SYMTAB */
  elfsh_get_sht(file, NULL);
  elfsh_get_symtab(file, NULL);

  /* Save the object */
  ret = elfsh_save_obj(file, OUTPUT_FILE);
  if (ret < 0)
    elfsh_error();

  printf("Relinking *%s* \n", ret ? "Error" : "OK");
  return (0);

}







