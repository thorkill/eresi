/*
** objects.c for elfsh
** 
** Started on  Mon Feb 24 12:21:12 2003 mayhem
** Last update Wed Apr 16 19:45:16 2003 mayhem
*/
#include "elfsh.h"




/* Now comes Level 1 objects hash functions */
elfshL1_t	*vm_create_L1ENT(void	*get_obj,
				 void	*get_obj_idx,
				 void	*get_obj_nam,
				 hash_t	*l2_hash,
				 void	*get_entptr,
				 void	*get_entval,
				 void	*set_entval,
				 u_int	elem_size)
{
  elfshL1_t	*new;

  XALLOC(new, sizeof (elfshL1_t), NULL);
  new->get_obj              = get_obj;
  new->get_obj_idx	    = get_obj_idx;
  new->get_obj_nam	    = get_obj_nam;
  new->get_entptr           = get_entptr;
  new->get_entval           = get_entval;
  new->set_entval           = set_entval;
  new->l2list               = l2_hash;
  new->elem_size	    = elem_size;
  return (new);
}


/* Now comes Level 2 objects hash functions */
elfshL2_t	*vm_create_L2ENT(void	*get_obj,
				 void	*set_obj,
				 char	type,
				 void	*get_name,
				 void	*set_name,
				 void	*get_data,
				 void	*set_data)
{
  elfshL2_t	*new;

  XALLOC(new, sizeof (elfshL1_t), NULL);
  new->get_obj  = get_obj;
  new->set_obj  = set_obj;
  new->get_name = get_name;
  new->set_name = set_name;
  new->type     = type;
  new->get_data = get_data;
  new->set_data = set_data;
  return (new);
}


/* Convert an elfshpath_t object in a different typed one */
int		vm_convert_object(elfshpath_t *obj, u_int objtype)
{
  u_long	val;

  if (obj->type != ELFSH_OBJINT || objtype != ELFSH_OBJSTR)
    ELFSH_SETERROR("[elfsh:convert_object] Requested conversion unsupported\n",
		   -1);
  val = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
  XALLOC(obj->immed_val.str, sizeof(val), -1);
  memcpy(obj->immed_val.str, &val, sizeof(val));
  obj->type = ELFSH_OBJSTR;
  obj->immed = 1;
  obj->size = sizeof(long);
  obj->sizelem = 0;
  return (0);
}

/* Verify an object sanity */
int		vm_check_object(elfshpath_t *pobj)
{
  switch (pobj->type)
    {
    case ELFSH_OBJRAW:
      if (pobj->immed == 1)
	break;
      if (pobj->get_data == NULL || pobj->set_data == NULL)
	ELFSH_SETERROR("[elfsh:check_object] Invalid object path\n", -1);
      break;
    case ELFSH_OBJSTR:
      if (pobj->immed == 1)
	break;
      if (pobj->get_name == NULL || pobj->set_name == NULL)
	ELFSH_SETERROR("[elfsh:check_object] Invalid object path\n", -1);
      break;
    case ELFSH_OBJINT:
      if (pobj->immed == 1)
	break;
      if (pobj->get_obj == NULL || pobj->set_obj == NULL)
	ELFSH_SETERROR("[elfsh:check_object] Invalid object path\n", -1);
      break;
    default:
      ELFSH_SETERROR("[elfsh:check_object] Unknown object type\n", -1);
    }
  return (0);
}
