/*
** quit.c for elfsh
** 
** Started on  Sat Jan 25 11:19:53 2003 mayhem
** Last update Fri Mar 21 10:27:18 2003 mayhem
*/
#include "elfsh.h"


/* Print an object */
void		vm_print_obj(elfshpath_t *obj)
{
  u_int		val;
  char		*str;

  switch (obj->type)
    {
    case ELFSH_OBJINT:
      val = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      printf("%08X ", val);
      break;
    case ELFSH_OBJSTR:
      str = (obj->immed ? obj->immed_val.str : 
	     obj->get_name(obj->root, obj->parent));
      printf("%s ", str);
      break;
    default:
      fprintf(stderr, "[elfsh:vm_print_obj] Unprintable object type\n");
    }
  fflush(stdout);
}



/* Print a string */
int			cmd_print()
{
  elfshpath_t		obj;
  volatile u_int	idx;
  volatile int		ret;

  for (idx = 0; world.args.param[idx] != NULL; idx++)
    {
      ret = vm_lookup_param(world.args.param[idx], &obj, 0);
      if (ret == -1)
	{
	  printf("%s ", world.args.param[idx]);
	  fflush(stdout);
	}
      else
	vm_print_obj(&obj);
    }

  puts("");
  if (!world.state.vm_quiet)
    puts("");
  return (0);
}
