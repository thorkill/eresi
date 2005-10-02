/*
** objects.c for elfsh
**
** Methods for creation of objects
**
** See lts.c for Lazy Type System implementation
** 
** Started on  Mon Feb 24 12:21:12 2003 mayhem
**
*/
#include "elfsh.h"


/* Create constant object : Perm == 1 if the object is writable */
elfshpath_t	*vm_create_IMMED(char type, char perm, u_int val)
{
  elfshpath_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Please use create_IMMEDSTR or create_LONG for that */
  if (type == ELFSH_OBJSTR || type == ELFSH_OBJLONG)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid type for immed", (NULL));

  //printf("created IMMED with val = %u \n", val);

  XALLOC(new, sizeof(elfshpath_t), NULL);
  new->immed = 1;
  new->perm = perm;
  new->type = type;
  new->size = 4;
  new->immed_val.word = val;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/* Create constant object */
elfshpath_t	*vm_create_LONG(char perm, elfsh_Addr val)
{
  elfshpath_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Please use create_IMMEDSTR for that */
  XALLOC(new, sizeof(elfshpath_t), NULL);
  new->immed = 1;
  new->perm = perm;
  new->type = ELFSH_OBJLONG;
  new->size = sizeof(val);
  new->immed_val.ent = val;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/* Create constant object */
elfshpath_t	*vm_create_SHORT(char perm, u_short val)
{
  elfshpath_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  //printf("created SHORT with val = %hu \n", val);

  /* Please use create_IMMEDSTR for that */
  XALLOC(new, sizeof(elfshpath_t), NULL);
  new->immed = 1;
  new->perm = perm;
  new->type = ELFSH_OBJSHORT;
  new->size = 2;
  new->immed_val.half = val;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/* Create constant object */
elfshpath_t	*vm_create_BYTE(char perm, u_char val)
{
  elfshpath_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  //printf("created BYTE with val = %hhu \n", val);

  /* Please use create_IMMEDSTR for that */
  XALLOC(new, sizeof(elfshpath_t), NULL);
  new->immed = 1;
  new->perm = perm;
  new->type = ELFSH_OBJBYTE;
  new->size = 1;
  new->immed_val.byte = val;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/* Create constant string object */
elfshpath_t	*vm_create_IMMEDSTR(char perm, char *str)
{
  elfshpath_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  //printf("created STR with val = %s \n", str);

  XALLOC(new, sizeof(elfshpath_t), NULL);
  new->immed = 1;
  new->perm = perm;
  new->type = ELFSH_OBJSTR;
  new->immed_val.str = str;
  new->size = strlen(new->immed_val.str);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/* Create a redirection abstract object */
elfshredir_t	*vm_create_REDIR(u_char type, char *sname, char *dname, 
				 elfsh_Addr saddr, elfsh_Addr daddr)
{
  elfshredir_t	*redir;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(redir, sizeof(elfshredir_t), NULL);
  redir->type   = type;
  redir->name[0] = sname;
  redir->name[1] = dname;
  redir->addr[0] = saddr;
  redir->addr[1] = daddr;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (redir));
}




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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(new, sizeof (elfshL1_t), NULL);
  new->get_obj              = get_obj;
  new->get_obj_idx	    = get_obj_idx;
  new->get_obj_nam	    = get_obj_nam;
  new->get_entptr           = get_entptr;
  new->get_entval           = get_entval;
  new->set_entval           = set_entval;
  new->l2list               = l2_hash;
  new->elem_size	    = elem_size;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(new, sizeof (elfshL1_t), NULL);
  new->get_obj  = get_obj;
  new->set_obj  = set_obj;
  new->get_name = get_name;
  new->set_name = set_name;
  new->type     = type;
  new->get_data = get_data;
  new->set_data = set_data;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}



/* The high level function for object conversion */
int		vm_convert_object(elfshpath_t *obj, u_int objtype)
{

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!obj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", (-1));

#if 0
  printf("[DEBUG_OBJECT] Trying to convert type %u into type %u \n", 
	 obj->type, objtype);
#endif  

  /* Automatic conversion */
  if (obj->type == ELFSH_OBJUNK || obj->type == objtype)
    {
      obj->type = objtype;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  switch (objtype)
    {
    case ELFSH_OBJSTR:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, vm_convert2str(obj));
    case ELFSH_OBJLONG:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, vm_convert2long(obj));
    case ELFSH_OBJINT:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, vm_convert2int(obj));
    case ELFSH_OBJBYTE:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, vm_convert2byte(obj));
    case ELFSH_OBJSHORT:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, vm_convert2short(obj));
    case ELFSH_OBJRAW:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, vm_convert2raw(obj));
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Destination type unknown", -1);
    }
}



/* Verify an object sanity */
elfshpath_t		*vm_check_object(elfshpath_t *pobj)
{
  char			buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  //printf("checking object %p of type %u \n", pobj, pobj->type);

  switch (pobj->type)
    {
    case ELFSH_OBJRAW:
      if (pobj->immed == 1)
	break;
    if (pobj->get_data == NULL || pobj->set_data == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid object path", NULL);
      break;
    case ELFSH_OBJSTR:
      if (pobj->immed == 1)
	break;
      if (pobj->get_name == NULL || pobj->set_name == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid object path", NULL);
      break;
    case ELFSH_OBJINT:
    case ELFSH_OBJLONG:
    case ELFSH_OBJSHORT:
    case ELFSH_OBJBYTE:
      if (pobj->immed == 1)
	break;
      if (pobj->get_obj == NULL || pobj->set_obj == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid object path", NULL);
      break;
    default:
      snprintf(buf, BUFSIZ, "[DEBUG_OBJECT] Failed to handle unknown object type = %u \n", pobj->type);
      vm_output(buf);
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown object type", NULL);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (pobj));
}

