/*
** test_usage.c for source level debugging tests in ERESI
**
** Initial version by Eduardo Franklin - 2009
** $Id$
*/
#include <libelfsh.h>
#include <libedfmt.h>


int			main(int ac, char **av)
{  
  elfshobj_t		*obj;
  edfmtdw2info_t	*info;
  int			to_ret;
  hash_t		*htable;
  char			**keys;
  int			keynbr;
  edfmtfunc_t		*lfunc;
  int			index;

  obj = elfsh_map_obj(av[1]);
  if (!obj)
    {
      printf("failed loading %s\n", av[1]);
      to_ret = -1;
      goto error_label;
    }
  edfmt_format(obj);
  edfmt_dwarf2_parse(obj);
  if (info = edfmt_dwarf2_getinfo(obj))
    printf("Succesfully loaded DWARF2 format information\n");
  else
    printf("Failed to load DWARF2 format information\n");

  htable = edfmt_hfuncs_get();
  keys = hash_get_keys(htable, &keynbr);
  printf("keynbr = %d\n", keynbr);
  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  if (htable == NULL || keys[index] == NULL)
	    {
	      if(htable == NULL)
		{
		  printf("htable == NULL\n");
		}
	      return (NULL);
	    }
	  lfunc = (edfmtfunc_t *) hash_get(htable, keys[index]);
	  printf("here edfmt_srcline_get will be called\n");
	}
    }

  to_ret = 0;
 error_label:
  return (to_ret);
}
