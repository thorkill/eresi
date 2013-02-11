/*
* @file libaspect/containers.c
** @ingroup libaspect
** 
** @brief Implement generic routines for containers.
**
** Started on  Sat Jun  2 15:20:18 2005 jfv
** $Id$
**
*/
#include "libaspect.h"


/**
 * @brief Create container lists
 * @param container Container holding the lists
 * @param linktype CONTAINER_LINK_IN or CONTAINER_LINK_OUT for input or output links list
 * @param uniqid Unique ID to be put into name
 * @return -1 on error and 0 on success
 */
int		container_linklists_create(container_t *container,
					   u_int	linktype,
					   u_int	uniqid)
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
      snprintf(bufname, BUFSIZ, "%d_%s_"AFMT"_%s", 
	       uniqid, prefix, *(eresi_Addr *) container->data, "inputs");
      newlist = elist_find(bufname);
      if (newlist)
	container->inlinks = newlist;
      else
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, container->inlinks, sizeof(list_t), -1);
	  elist_init(container->inlinks, strdup(bufname), ASPECT_TYPE_LINK);
	}

      break;
    case CONTAINER_LINK_OUT:
      snprintf(bufname, BUFSIZ, "%d_%s_"AFMT"_%s", 
	       uniqid, prefix, *(eresi_Addr *) container->data, "outputs");
      newlist = elist_find(bufname);
      if (newlist)
	container->outlinks = newlist;
      else
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, container->outlinks, sizeof(list_t), -1);
	  elist_init(container->outlinks, strdup(bufname), ASPECT_TYPE_LINK);
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
 * @param outlist Output links list if any (else it will be created empty)
 * @param uniqid Unique ID to be put into name
 * @return Container newly created
 */
container_t	*container_create(u_int type, void *data, list_t *inlist, list_t *outlist, u_int uniqid)
{
  container_t	*newcntnr;
  aspectype_t	*rtype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  rtype = aspect_type_get_by_id(type);
  if (!rtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unknown container element type", NULL);  

  /* Make sure meta-data is initialized and contiguous with pointed data */
#if defined(DEBUG_LIBASPECT)
  fprintf(stderr, "Allocating sizeof(container) + (%s type->size = %u) \n", 
	  rtype->name, rtype->size);
#endif
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newcntnr, sizeof(container_t) + rtype->size, NULL);
  newcntnr->data = (char *) newcntnr + sizeof(container_t);
  newcntnr->type = type;
  memcpy((char *) newcntnr->data, (char *) data, rtype->size);

  /* Create lists if not specified */
  if (inlist)
    newcntnr->inlinks = elist_copy(inlist, ELIST_DATA_COPY);
  else
    container_linklists_create(newcntnr, CONTAINER_LINK_IN, uniqid);
  if (outlist)
    newcntnr->outlinks = elist_copy(outlist, ELIST_DATA_COPY);
  else
    container_linklists_create(newcntnr, CONTAINER_LINK_OUT, uniqid);

  /* Make sure the container and contained data are contiguous */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}
