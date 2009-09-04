#include <libelfsh.h>
#include <libedfmt.h>


int	main(int ac, char **av)
{
  
  elfshobj_t		*obj;
  edfmtdw2info_t	*info;
  int			to_ret;

  obj = elfsh_map_obj(av[0]);
  if (!obj)
    {
      printf("failed loading %s\n", av[0]);
      to_ret = -1;
      goto error_label;
    }
  edfmt_format(obj);
  edfmt_dwarf2_parse(obj);
  if (info = edfmt_dwarf2_getinfo(obj))
    printf("Succesfully loaded DWARF2 format information\n");
  else
    printf("Failed to load DWARF2 format information\n");
    
  to_ret = 0;
 error_label:
  return (to_ret);
}
