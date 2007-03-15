/*
 * 2001-2007 Devhell Labs
 * 2007 Asgard Labs, thorolf
 * 
 * Container related API
 *
 * $Id: container.c,v 1.3 2007-03-15 13:11:48 thor Exp $
 *
 */

#include "libmjollnir.h"

static mjrcontainer_t **reg_containers;
static unsigned int cntnrs_size = MJR_CNTNRS_INCREMENT;
static unsigned int next_id = 1;

/**
 *
 */
void mjr_init_containers()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, reg_containers, 
	 sizeof(mjrcontainer_t*) * cntnrs_size, NULL);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 *
 */
void mjr_resize_containers()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  cntnrs_size += MJR_CNTNRS_INCREMENT;
  XREALLOC(__FILE__, __FUNCTION__, __LINE__, reg_containers, reg_containers,
	   sizeof(mjrcontainer_t*) * cntnrs_size, NULL);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 *
 */
unsigned int mjr_register_container(mjrcontainer_t *cntnr)
{
  if (next_id >= cntnrs_size)
    {
      if (cntnrs_size < MJR_CNTNRS_INCREMENT * MJR_MAX_INCREMENTS)
	{
	  mjr_resize_containers();
	}
      else
	{
	  return 0;
	}
    }
  
  reg_containers[next_id] = cntnr;
  cntnr->id = next_id;
  return next_id++;
}

/**
 *
 */
unsigned int mjr_register_container_id (mjrcontainer_t *cntnr)
{
  if (cntnr->id >= cntnrs_size)
    {
      if (cntnrs_size < MJR_CNTNRS_INCREMENT * MJR_MAX_INCREMENTS)
	{
	  mjr_resize_containers();
	}
      else
	{
	  return 0;
	}
    }
  
  reg_containers[cntnr->id] = cntnr;
  if (cntnr->id >= next_id)
    next_id = cntnr->id + 1;
  
  return cntnr->id;
}

/**
 *
 */
void mjr_unregister_container (unsigned int id)
{
	reg_containers[id] = NULL;
}

/**
 *
 */
mjrcontainer_t *mjr_lookup_container (unsigned int id)
{
	return reg_containers[id];
}

/**
 *
 */
mjrlink_t *mjr_container_add_link (mjrcontainer_t *cntnr,
				   unsigned int id,
				   int link_type,
				   int link_direction)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  mjrlink_t *cur;
  mjrlink_t *link;
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__, link, sizeof(mjrlink_t), NULL );
  link->id = id;
  link->type = link_type;
  link->next = NULL;
  
  if (link_direction == MJR_LINK_IN) 
    {
      cur = cntnr->input;
      cntnr->in_nbr++;
      if (!cur)
	{
	  cntnr->input = link;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, link);
	}
    }
  else if (link_direction == MJR_LINK_OUT)
    {
      cur = cntnr->output;
      cntnr->out_nbr++;
      if (!cur)
	{
	  cntnr->output = link;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, link);
	}
    }
  
  while (cur->next)
    cur = cur->next;
  
  cur->next = link;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, link);
}

/**
 *
 */
mjrlink_t *mjr_get_link_of_type(mjrlink_t *link, int link_type)
{
  if (!link || link->type == link_type)
    return link;
  else 
    return mjr_get_link_of_type(link->next, link_type);
}

/**
 *
 */
mjrcontainer_t *mjr_create_block_container(u_int symoff,
					   elfsh_Addr vaddr,
					   u_int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mjrblock_t 			*newblock;
  mjrcontainer_t	*newcntnr;
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newblock, sizeof(mjrblock_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newcntnr, sizeof(mjrcontainer_t), NULL);
  
  newblock->symoff = symoff;
  newblock->vaddr = vaddr;
  newblock->size = size;
  
  newcntnr->data = newblock;
  newcntnr->type = MJR_CNTNR_BLOCK;
  newcntnr->input = NULL;
  newcntnr->in_nbr = 0;
  newcntnr->output = NULL;
  newcntnr->out_nbr = 0;
  
  mjr_register_container(newcntnr);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}

/**
 * Creates function container
 */
mjrcontainer_t *mjr_create_function_container(elfsh_Addr vaddr,
					      u_int size,
					      char *name,
					      mjrblock_t *first,
					      char *md5)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mjrfunc_t		*newfunction;
  mjrcontainer_t	*newcntnr;
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newfunction, sizeof(mjrfunc_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newcntnr, sizeof(mjrcontainer_t), NULL);
  
  newfunction->vaddr = vaddr;
  newfunction->size = size;
  if (name)
    strncpy(newfunction->name, name, sizeof(newfunction->name)-1);
  else
    newfunction->name[0] = 0;
  
  newfunction->first = NULL;
  if (md5)
    memcpy(newfunction->md5, md5, sizeof(newfunction->md5));
  else
    memset(newfunction->md5, 0, sizeof(newfunction->md5));
  
  newcntnr->data = newfunction;
  newcntnr->type = MJR_CNTNR_FUNC;
  newcntnr->input = NULL;
  newcntnr->in_nbr = 0;
  newcntnr->output = NULL;
  newcntnr->out_nbr = 0;
  
  mjr_register_container(newcntnr);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}

