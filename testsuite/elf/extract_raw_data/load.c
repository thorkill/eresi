#include <stdio.h>
#include "libelfsh.h"

int test = 42;

/**
 *
 *
 */
int	main(int ac, char **av)
{
  int	idx;
  
  char *targets[] =
    {
#if ERESI32
      "load32",
      "load.32.o",
#elif ERESI64
      "load.64.o.64",
#endif
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
  elfshobj_t	*obj;
  void		*ptr;
  eresi_Addr	start;
  unsigned long len;
  int		ret;
  int		foffset;
  int		to_ret;
  elfshsect_t	*sect;
  char		*sym_name = ".data";

  /** Load elfsh objet */
  obj = elfsh_load_obj(binary);
  if (!obj)
    {
      printf("ERR failed loading object %s \n", binary);
      return(0);
    }

  /** Get file offset from that symbol */
  sect = elfsh_get_section_by_name(obj, sym_name, NULL, NULL, NULL);
  if (!sect)
    {
      printf("ERR failed loading section %s", sym_name);
      return (-1);
    }

  foffset = sect->shdr->sh_offset;
  len = sect->shdr->sh_size;
  printf("INFO file offset of %s = %u len = %u\n", sym_name, foffset, len);

  ptr = malloc(len);
  ret = elfsh_readmemf(obj, foffset, ptr, len);
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

