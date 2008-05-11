/*
** @file containers.c
** @ingroup libaspect
** 
** @brief Implement generic routines for containers.
**
** Started on  Sat Jun  2 15:20:18 2005 jfv
** $Id: containers.c,v 1.3 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libaspect.h"


/**
 * @brief Create container lists
 * @param container Container holding the lists
 * @param linktype CONTAINER_LINK_IN or CONTAINER_LINK_OUT for input or output links list
 * @return -1 on error and 0 on success
 */
int		container_linklists_create(container_t *container,
					   u_int	linktype)
{
  aspectype_t  *type;
  char		bufname[BUFSIZ];
  char		*prefix;
  list_t	*newlist;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check for prefix (XXX: change to lookup user-configured prefixes ?) */
  switch (container->type)
    {
    case ASPECT_TYPE_BLOC:
      prefix = "bloc";
      break;
    case ASPECT_TYPE_FUNC:
      prefix = "func";
      break;
    default:
      type = aspect_type_get_by_id(container->type);
      if (!type)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to find type of container", -1);
      prefix = type->name;
    }

  /* Now really allocate the list */
  switch (linktype)
    {
    case CONTAINER_LINK_IN:
      snprintf(bufname, BUFSIZ, "%s_%08X_%s", prefix, *(u_long *) container->data, "inputs");
      newlist = elist_find(bufname);
      if (newlist)
	container->inlinks = newlist;
      else
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, container->inlinks, sizeof(list_t), -1);
	  elist_init(container->inlinks, strdup(bufname), container->type);
	}

      break;
    case CONTAINER_LINK_OUT:
      snprintf(bufname, BUFSIZ, "%s_%08X_%s", prefix, *(u_long *) container->data, "outputs");
      newlist = elist_find(bufname);
      if (newlist)
	container->outlinks = newlist;
      else
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, container->outlinks, sizeof(list_t), -1);
	  elist_init(container->outlinks, strdup(bufname), container->type);
	}
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unknown link type", -1);  
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Create a new container
 * @param type Type of element inside container
 * @param data Data pointer for contained element
 * @param inlist Input links list if any (else it will be created empty)
 * @Param outlist Output links list if any (else it will be created empty)
 * @return Container newly created
 */
container_t	*container_create(u_int type, void *data, list_t *inlist, list_t *outlist)
{
  container_t	*newcntnr;
  aspectype_t	*rtype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  rtype = aspect_type_get_by_id(type);
  if (!rtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown container element type", NULL);  

  /* Make sure meta-data is initialized and contiguous with pointed data */
  fprintf(stderr, "Allocating sizeof(container) + (%s type->size = %u) \n", 
	  rtype->name, rtype->size);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, newcntnr, sizeof(container_t) + rtype->size, NULL);
  newcntnr->data = (char *) newcntnr + sizeof(container_t);
  newcntnr->type = type;
  memcpy((char *) newcntnr->data, (char *) data, rtype->size);

  /* Create lists if not specified */
  if (inlist)
    newcntnr->inlinks = elist_copy(inlist);
  else
    container_linklists_create(newcntnr, CONTAINER_LINK_IN);
  if (outlist)
    newcntnr->outlinks = elist_copy(outlist);
  else
    container_linklists_create(newcntnr, CONTAINER_LINK_OUT);
  
  /* Make sure the container and contained data are contiguous */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}
