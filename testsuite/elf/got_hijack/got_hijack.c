/*
** unlink.c for unlinked
** 
** Started on  Sun Mar 10 01:18:51 2002 jfv
** Last update Sun Mar 10 02:46:18 2002 jfv
**
** $Id: got_hijack.c,v 1.3 2007-07-31 03:28:48 may Exp $
**
*/
#include	"libelfsh.h"


#if ERESI32
 #define		TROJANED_FILE	"./hijackme32"
 #define		OUTPUT_FILE	"./fake_aout32"
#elif ERESI64
 #define		TROJANED_FILE	"./hijackme64"
 #define		OUTPUT_FILE	"./fake_aout64"
#endif

int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  elfsh_Sym	*new_sleep;
  int		ret;

  file = elfsh_map_obj(TROJANED_FILE);
  new_sleep = elfsh_get_symbol_by_name(file, "new_sleep");
  if (!file || !new_sleep)
    {
      elfsh_error();
      exit(-1);
    }

  /* Try this to hijack got by index or the second one to hijack by name */
  //ret = elfsh_set_got_entry_by_index(file, 5, new_sleep->st_value);
  ret = elfsh_set_got_entry_by_name(file, "sleep", new_sleep->st_value);
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

