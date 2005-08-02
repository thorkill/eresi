/*
** quit.c for elfsh
** 
** Started on  Sat Jan 25 11:19:53 2003 mayhem
** 
*/
#include "elfsh.h"


/* Print an object */
void		vm_print_obj(elfshpath_t *obj)
{
  char		byte;
  uint16_t	half;
  uint32_t	word;
  elfsh_Addr   	val;
  char		*str;
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  bzero(logbuf, BUFSIZ);
  switch (obj->type)
    {
    case ELFSH_OBJBYTE:
      byte = (obj->immed ? obj->immed_val.byte : obj->get_obj(obj->parent));
      snprintf(logbuf, BUFSIZ - 1, "%c (%hhd) ", byte, byte);
    case ELFSH_OBJSHORT:
      half = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
      snprintf(logbuf, BUFSIZ - 1, "%04hx (%hd) ", half, half);
    case ELFSH_OBJLONG:
      val = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      snprintf(logbuf, BUFSIZ - 1, XFMT " (" DFMT ") ", val, val);
    case ELFSH_OBJINT:
      word = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
      snprintf(logbuf, BUFSIZ - 1, "%08X (%d) ", word, word);
      break;
    case ELFSH_OBJSTR:
      str = (obj->immed ? obj->immed_val.str : 
	     obj->get_name(obj->root, obj->parent));
      snprintf(logbuf, BUFSIZ - 1, "%s ", str);
      break;
    default:
      snprintf(logbuf, BUFSIZ - 1,
	       "[elfsh:vm_print_obj] Unprintable object type %u\n",
	       obj->type);
    }
  vm_output(logbuf);
}



/* Print a string */
int			cmd_print()
{
  elfshpath_t		*obj;
  volatile u_int	idx;
  char			logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (idx = 0; world.curjob->curcmd->param[idx] != NULL; idx++)
    {
      obj = vm_lookup_param(world.curjob->curcmd->param[idx], 1);
      if (!obj)
	{
	  snprintf(logbuf, BUFSIZ - 1, "%s ", world.curjob->curcmd->param[idx]);
	  vm_output(logbuf);
	  continue;
	}
      vm_print_obj(obj);
    }

  vm_output("\n");
  if (!world.state.vm_quiet)
    vm_output("\n");
  return (0);
}
