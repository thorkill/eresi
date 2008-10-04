/**
 * @file objects.c
 * @ingroup librevm_lang
 * @brief Methods for creation of objects.
 *
 * See lts.c for Lazy Type System implementation
 * 
 * Started on  Mon Feb 24 12:21:12 2003 jfv
 *
 *
 * $Id: objects.c,v 1.13 2007-11-28 07:56:09 may Exp $
 *
 */
#include "revm.h"


/**
 * @brief Create constant object : Perm == 1 if the object is writable 
 */
revmobj_t	*revm_create_IMMED(char type, char perm, unsigned int val)
{
  revmobj_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Please use create_IMMEDSTR or create_LONG for that */
  if (type == ASPECT_TYPE_STR || type == ASPECT_TYPE_CADDR ||
      type == ASPECT_TYPE_CADDR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid type for immed", (NULL));

  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(revmobj_t), NULL);
  new->immed = 1;
  new->perm  = perm;
  new->otype = aspect_type_get_by_id(type);
  new->size  = 4;
  new->immed_val.word = val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/**
 * @brief Create constant object 
 */
revmobj_t	*revm_create_LONG(char perm, eresi_Addr val)
{
  revmobj_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Please use create_IMMEDSTR for that */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(revmobj_t), NULL);
  new->immed = 1;
  new->perm  = perm;
  new->otype = aspect_type_get_by_id(ASPECT_TYPE_LONG);
  new->size  = sizeof(val);
  new->immed_val.ent = val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/** 
 * @brief Create constant object 
 */
revmobj_t	*revm_create_CADDR(char perm, eresi_Addr val)
{
  revmobj_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Please use create_IMMEDSTR for that */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(revmobj_t), NULL);
  new->immed = 1;
  new->perm  = perm;
  new->otype = aspect_type_get_by_id(ASPECT_TYPE_CADDR);
  new->size  = sizeof(val);
  new->immed_val.ent = val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/** 
 * @brief Create constant object
 *
 * @param perm
 * @param val
 * @return
 */
revmobj_t	*revm_create_DADDR(char perm, eresi_Addr val)
{
  revmobj_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Please use create_IMMEDSTR for that */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(revmobj_t), NULL);
  new->immed = 1;
  new->perm = perm;
  new->otype = aspect_type_get_by_id(ASPECT_TYPE_DADDR);
  new->size = sizeof(val);
  new->immed_val.ent = val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/** 
 * @brief Create constant object 
 */
revmobj_t	*revm_create_SHORT(char perm, u_short val)
{
  revmobj_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //printf("created SHORT with val = %hu \n", val);

  /* Please use create_IMMEDSTR for that */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(revmobj_t), NULL);
  new->immed = 1;
  new->perm = perm;
  new->otype = aspect_type_get_by_id(ASPECT_TYPE_SHORT);
  new->size = 2;
  new->immed_val.half = val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/** 
 * @brief Create constant object 
 */
revmobj_t	*revm_create_BYTE(char perm, unsigned char val)
{
  revmobj_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //printf("created BYTE with val = %hhu \n", val);

  /* Please use create_IMMEDSTR for that */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(revmobj_t), NULL);
  new->immed = 1;
  new->perm = perm;
  new->otype = aspect_type_get_by_id(ASPECT_TYPE_BYTE);
  new->size = 1;
  new->immed_val.byte = val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/** 
 * @brief Create constant string object 
 */
revmobj_t	*revm_create_IMMEDSTR(char perm, char *str)
{
  revmobj_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //printf("created STR with val = %s \n", str);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(revmobj_t), NULL);
  new->immed = 1;
  new->perm = perm;
  new->otype = aspect_type_get_by_id(ASPECT_TYPE_STR);
  new->immed_val.str = str;
  new->size = strlen(new->immed_val.str);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}

/** 
 * @brief Create a redirection abstract object 
 */
elfshredir_t	*revm_create_REDIR(unsigned char type, char *sname, char *dname, 
				 eresi_Addr saddr, eresi_Addr daddr)
{
  elfshredir_t	*redir;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,redir, sizeof(elfshredir_t), NULL);
  redir->type   = type;
  redir->name[0] = sname;
  redir->name[1] = dname;
  redir->addr[0] = saddr;
  redir->addr[1] = daddr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (redir));
}




/** 
 * @brief Now comes Level 1 objects hash functions 
 */
revmL1_t	*revm_create_L1ENT(void	*get_obj,
				 void	*get_obj_idx,
				 void	*get_obj_nam,
				 hash_t	*l2_hash,
				 void	*get_entptr,
				 void	*get_entval,
				 void	*set_entval,
				 unsigned int	elem_size)
{
  revmL1_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof (revmL1_t), NULL);
  new->get_obj              = get_obj;
  new->get_obj_idx	    = get_obj_idx;
  new->get_obj_nam	    = get_obj_nam;
  new->get_entptr           = get_entptr;
  new->get_entval           = get_entval;
  new->set_entval           = set_entval;
  new->l2list               = l2_hash;
  new->elem_size	    = elem_size;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/** 
 * Now comes Level 2 objects hash functions 
 */
revmL2_t	*revm_create_L2ENT(void	*get_obj,
				 void	*set_obj,
				 char	type,
				 void	*get_name,
				 void	*set_name,
				 void	*get_data,
				 void	*set_data)
{
  revmL2_t	*new;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof (revmL1_t), NULL);
  new->get_obj  = get_obj;
  new->set_obj  = set_obj;
  new->get_name = get_name;
  new->set_name = set_name;
  new->type     = type;
  new->get_data = get_data;
  new->set_data = set_data;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}



/**
 * @brief The high level function for object conversion 
 */
int		revm_convert_object(revmexpr_t *expr, unsigned int objtype)
{
  aspectype_t	*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!expr || !expr->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", (-1));
  type = aspect_type_get_by_id(objtype);
  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter type", (-1));

#if 0
  printf("[DEBUG_OBJECT] Trying to convert type %u into type %u \n", 
	 (expr->type ? expr->type->type : 0), objtype);
#endif  

  /* Automatic conversion */
  if (!expr->type)
    {
      expr->type         = type;
      if (expr->value)
	expr->value->otype = expr->type;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  if (expr->type->type == objtype)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  expr->type = type;
  switch (objtype)
    {
    case ASPECT_TYPE_STR:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, revm_convert2str(expr->value));
    case ASPECT_TYPE_CADDR:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, revm_convert2caddr(expr->value));
    case ASPECT_TYPE_DADDR:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, revm_convert2daddr(expr->value));
    case ASPECT_TYPE_LONG:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, revm_convert2long(expr->value));
    case ASPECT_TYPE_INT:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, revm_convert2int(expr->value));
    case ASPECT_TYPE_BYTE:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, revm_convert2byte(expr->value));
    case ASPECT_TYPE_SHORT:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, revm_convert2short(expr->value));
    case ASPECT_TYPE_RAW:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, revm_convert2raw(expr->value));
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Destination type unknown", -1);
    }
}



/** 
 * @brief Verify an object sanity 
 */
revmobj_t		*revm_check_object(revmobj_t *pobj)
{
  char			buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //printf("checking object %p of type %u \n", pobj, pobj->type);

  switch (pobj->otype->type)
    {
    case ASPECT_TYPE_RAW:
      if (pobj->immed == 1)
	break;
    if (pobj->get_data == NULL || pobj->set_data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid object path", NULL);
      break;
    case ASPECT_TYPE_STR:
      if (pobj->immed == 1)
	break;
      if (pobj->get_name == NULL || pobj->set_name == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid object path", NULL);
      break;
    case ASPECT_TYPE_BYTE:
    case ASPECT_TYPE_SHORT:
    case ASPECT_TYPE_INT:
    case ASPECT_TYPE_LONG:
    case ASPECT_TYPE_CADDR:
    case ASPECT_TYPE_DADDR:
      if (pobj->immed == 1)
	break;
      if (pobj->get_obj == NULL || pobj->set_obj == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid object path", NULL);
      break;
    default:
      snprintf(buf, BUFSIZ, "[DEBUG_OBJECT] Failed to handle unknown object type = %u \n", 
	       pobj->otype->type);
      revm_output(buf);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown object type", NULL);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (pobj));
}


/** 
 * @brief Destroy an object 
 */
void		revm_destroy_object(revmobj_t *pobj)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (pobj->otype->type == ASPECT_TYPE_STR && pobj->immed)
    XFREE(__FILE__, __FUNCTION__, __LINE__, pobj->immed_val.str);
  XFREE(__FILE__, __FUNCTION__, __LINE__, pobj);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/** 
 * @brief Destroy an object 
 */
revmobj_t	 *revm_copy_object(revmobj_t *pobj)
{
  revmobj_t	*copy;
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  str = NULL;
  if (!pobj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", NULL);
  if (pobj->otype->type == ASPECT_TYPE_STR && pobj->immed)
    str = strdup(pobj->immed_val.str);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, copy, sizeof(revmobj_t), NULL);
  memcpy(copy, pobj, sizeof(revmobj_t));
  if (str)
    copy->immed_val.str = str;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, copy);
}
