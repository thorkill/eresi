#include <stdio.h>
#include "libelfsh.h"

int	test_binary(char *bin);

/**
 *
 *
 */
int	main(int ac, char **av)
{
  int	idx;
  
  char *targets[] =
    {
      "load",
      "load.o",
      NULL
    };
  
  if (ac > 1)
    test_binary(av[1]);
  else
    {
      for (idx = 0; targets[idx] != NULL; idx++)
	{
	  if (!(test_binary(targets[idx])))
	    printf("ERR on %s\n", targets[idx]);
	}
    }
}


int	test_binary(char *binary)
{
  Elf32_Sym	*sym;
  elfshobj_t	*obj;
  void		*ptr;
  unsigned long	start;
  unsigned long len;
  int		ret;
  int		foffset;
  int		to_ret;
  elfshsect_t	*sect;

  char	*sym_name = ".text";

  /** Load elfsh objet */
  obj = elfsh_load_obj(binary);
  if (!obj)
    {
      printf("ERR failed loading object %s", binary);
      return(0);
    }
  /** Fetch symbol from a string */
  sym = elfsh_get_metasym_by_name(obj,sym_name);
  if (!sym)
    {
      printf("ERR add elfsh handling error there : metasym failed\n");
      to_ret = 0;
      goto leave;
    }
  
  len = sym->st_size;

  /** Get file offset from that symbol */
  printf("INFO vaddr = %08X size = %i\n", sym->st_value, sym->st_size);
  sect = elfsh_get_section_by_name(obj, sym_name, NULL, NULL, NULL);
  if (!sect)
    {
      printf("ERR failed loading section %s", sym_name);
      return (-1);
    }

  foffset = sect->shdr->sh_offset;
  printf("INFO file offset of %s = %08X\n", sym_name, foffset);

  ptr = malloc(len);
  ret = elfsh_raw_read(obj, foffset, ptr, len);
  printf("INFO raw_read returned %i\n", ret);
  if (len != ret)
    {
      printf("ERR error : read only %i bytes on %i\n", ret, len);
      profiler_error();
      to_ret = 0;
      goto leave;
    }
  
  to_ret = 1;
 leave:
  elfsh_unload_obj(obj);
  return (to_ret);
}

