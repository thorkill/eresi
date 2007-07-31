/*
 * 2001-2007 Devhell Labs, Rfdslabs, Asgardlabs
 * 
 * An API for generic containers data structures
 *
 * $Id: container.c,v 1.15 2007-07-31 23:30:35 may Exp $
 */
#include "libmjollnir.h"

/**
 * Initialize containers for context
 * @param ctx mjollnir context
 */
int	mjr_init_containers(mjrcontext_t *ctx)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  fprintf(stderr, " ************* Initialized containers (file %s) ! \n",
	  ctx->obj->name);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, ctx->reg_containers, 
	 sizeof(mjrcontainer_t*) * ctx->cntnrs_size, -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Resize containers for given context
 * @param ctx mjollnir context
 */
int	mjr_resize_containers(mjrcontext_t *ctx, unsigned int resize)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ctx->cntnrs_size += (resize ? resize + 1 : MJR_CNTNRS_INCREMENT);
  XREALLOC(__FILE__, __FUNCTION__, __LINE__, 
	   ctx->reg_containers, ctx->reg_containers,
	   sizeof(mjrcontainer_t*) * ctx->cntnrs_size, -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Reigster new container for given context
 * @param ctx mjollnir context
 * @param cntnr container
 */
unsigned int mjr_register_container(mjrcontext_t *ctx, mjrcontainer_t *cntnr)
{

  /* Allocate more memory if we lack some */
  if (ctx->next_id >= ctx->cntnrs_size)
    {
      if (ctx->cntnrs_size < MJR_CNTNRS_INCREMENT * MJR_MAX_INCREMENTS)
	mjr_resize_containers(ctx, 0);
      else
	return 0;
    }
  
  ctx->reg_containers[ctx->next_id] = cntnr;
  cntnr->id = ctx->next_id;

#if __DEBUG_CNTNR__
  fprintf(D_DESC,"[D] %s: %x registred id: %d type: %d\n",
	  __FUNCTION__, *(elfsh_Addr *) cntnr->data, cntnr->id, cntnr->type);
#endif

  return ctx->next_id++;
}

/**
 * Register container in context by id
 * @param ctx mjollnir context
 */
unsigned int mjr_register_container_id(mjrcontext_t *ctx, mjrcontainer_t *cntnr)
{

  /* When reloading containers from on-disk, containers can be stored in any ID order, thus
     we need to make sure we allocate sufficiently room, not just an atomic alloc increment */
  if (cntnr->id >= ctx->cntnrs_size)
    {
      if (ctx->cntnrs_size < MJR_CNTNRS_INCREMENT * MJR_MAX_INCREMENTS)
	mjr_resize_containers(ctx, (cntnr->id - ctx->cntnrs_size));
      else
	return 0;
    }
  
  ctx->reg_containers[cntnr->id] = cntnr;
  if (cntnr->id >= ctx->next_id)
    ctx->next_id = cntnr->id + 1;

#if __DEBUG_CNTNR__
  fprintf(D_DESC,"[D] %s: %x registred id: %d\n",
	  __FUNCTION__, *(elfsh_Addr *) cntnr->data, cntnr->id);
#endif

  return cntnr->id;
}

/**
 * Unregister container by id
 * @param ctx mjollnir context
 * @param id container id
 */
void mjr_unregister_container(mjrcontext_t *ctx, unsigned int id)
{
  ctx->reg_containers[id] = NULL;
}

/**
 * Lookup container by id
 * @param ctx mjollnir context
 * @param id container id
 */
mjrcontainer_t *mjr_lookup_container(mjrcontext_t *ctx, unsigned int id)
{
  return ctx->reg_containers[id];
}


/**
 * Add a generic link (input or output) to a container
 */
mjrlink_t	*mjr_container_add_link(mjrcontext_t	*ctx,
					mjrcontainer_t  *cntnr,
					unsigned int	id,
					int		link_type,
					int		link_direction)
{
  list_t	*linklist;
  listent_t	*listent;
  listent_t	*savednext;
  mjrlink_t	*link;
  mjrcontainer_t *cnt;
  char		linkname[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_CNTNR__
  if (cntnr->id == id)
    fprintf(D_DESC, "[D] %s: linking the same container id:%d\n",
	    __FUNCTION__, id);
#endif

  /* Check if we are linked with container id by other link type */
  linklist = mjr_link_get_by_direction(cntnr, link_direction);
  for (listent = linklist->head; listent; listent = savednext)
    {
      link      = (mjrlink_t *) listent->data;
      cnt       = ctx->reg_containers[link->id];
      savednext = listent->next;

      if (cnt->type == cntnr->type && link->id == id)
	{

#if __DEBUG_CNTNR__
	  fprintf(D_DESC, "[D] %s: already linked with id:%d type:%d/%d\n",
		  __FUNCTION__, id, link->type, link_type);
#endif

	  if (link->type == link_type)
	    {

#if __DEBUG_CNTNR__
	      fprintf(D_DESC,"[D] %s: return existing link id:%d\n", __FUNCTION__, id);
#endif

	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, link);
	    }

	  else if ((link->type == MJR_LINK_BLOCK_COND_ALWAYS) &&
		   ((link_type == MJR_LINK_BLOCK_COND_TRUE) ||
		    (link_type == MJR_LINK_BLOCK_COND_FALSE)))
	    {

#if __DEBUG_CNTNR__
	      fprintf(D_DESC,"[D] %s: removing old link id:%d from type:%d to %d\n",
		      __FUNCTION__,id, link->type,link_type);
#endif

	      list_del(linklist, listent->key);
	    }
	}
    }

#if __DEBUG_CNTNR__
  fprintf(D_DESC,"[D] %s: link id:%d -> id:%d type:%d dir:%d\n",
	  __FUNCTION__, cntnr->id, id, link_type, link_direction);
#endif

  /* Create and add the new link */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, link, sizeof(mjrlink_t), NULL);
  link->id   = id;
  link->type = link_type;
  snprintf(linkname, sizeof(linkname), "link_%u", id);
  list_add(linklist, strdup(linkname), link);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, link);
}



/**
 * Return a link giving its type
 */
mjrlink_t	*mjr_get_link_by_type(list_t *listlink, int link_type)
{
  listent_t	*ent;
  mjrlink_t	*curlink;

  if (!listlink || !listlink->head)
    return (NULL);
  for (ent = listlink->head; ent; ent = ent->next)
    {
      curlink = (mjrlink_t *) ent->data;
      if (curlink->type == link_type)
	return curlink;
    }
  return (NULL);
}


/**
 *
 */
int		match_block(void *elem, void *match)
{
  mjrblock_t	*blk_elem, *blk_match;

  blk_elem = (mjrblock_t *)((mjrcontainer_t *)elem)->data;
  blk_match = (mjrblock_t *)((mjrcontainer_t *)match)->data;

  return (blk_match->vaddr - blk_elem->vaddr);
}


/**
 * Create container lists
 */
int			mjr_create_container_linklist(mjrcontainer_t *container,
						      u_int	      linktype)
{
  char			bufname[BUFSIZ];
  char			*prefix;

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
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unknown container type", -1);  
    }
  
  /* Now really allocate the list */
  switch (linktype)
    {
    case MJR_LINK_IN:
      snprintf(bufname, BUFSIZ, "%s_%08X_%s", prefix, *(elfsh_Addr *) container->data, "inputs");
      XALLOC(__FILE__, __FUNCTION__, __LINE__, container->inlinks, sizeof(list_t), 0);
      list_init(container->inlinks, strdup(bufname), container->type);
      break;
    case MJR_LINK_OUT:
      snprintf(bufname, BUFSIZ, "%s_%08X_%s", prefix, *(elfsh_Addr *) container->data, "outputs");
      XALLOC(__FILE__, __FUNCTION__, __LINE__, container->outlinks, sizeof(list_t), 0);
      list_init(container->outlinks, strdup(bufname), container->type);
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unknown link type", -1);  
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Creates a block container
 */
mjrcontainer_t		*mjr_create_block_container(mjrcontext_t	*ctx,
						    u_int		symoff,
						    elfsh_Addr		vaddr,
						    u_int		size)
{
  mjrblock_t 		*newblock;
  mjrcontainer_t	*newcntnr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_CNTNR__
  newcntnr = mjr_get_container_by_vaddr(ctx, vaddr, ASPECT_TYPE_BLOC);
  if (newcntnr)
    {
      fprintf(D_DESC,"[D] %s: block container %x id:%d is there\n",
	      __FUNCTION__, vaddr, newcntnr->id);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
    }
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 newblock, sizeof(mjrblock_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 newcntnr, sizeof(mjrcontainer_t), NULL);

  newblock->symoff = symoff;
  newblock->vaddr = vaddr;
  newblock->size = size;
  newcntnr->data = newblock;
  newcntnr->type = ASPECT_TYPE_BLOC;
  mjr_create_container_linklist(newcntnr, MJR_LINK_IN);
  mjr_create_container_linklist(newcntnr, MJR_LINK_OUT);

#if __DEBUG_CNTNR__
  fprintf(D_DESC,"[D] %s: create block %x (%d)\n",
	  __FUNCTION__, vaddr, size);
#endif
  mjr_register_container(ctx, newcntnr);

  /* fill the btree */
  btree_insert_sort(&ctx->block_btree, match_block, newcntnr);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}


/**
 * Creates function container
 */
mjrcontainer_t		*mjr_create_function_container(mjrcontext_t	*ctx,
						       elfsh_Addr	vaddr,
						       u_int		size,
						       char		*name,
						       mjrblock_t	*first,
						       char		*md5)
{
  mjrfunc_t		*newfunction;
  mjrcontainer_t	*newcntnr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Allocate the new container and its links lists */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 newfunction, sizeof(mjrfunc_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 newcntnr, sizeof(mjrcontainer_t), NULL);
  newcntnr->type     = ASPECT_TYPE_FUNC;
  newfunction->vaddr = vaddr;
  newfunction->size  = size;
  newcntnr->data     = newfunction;
  mjr_create_container_linklist(newcntnr, MJR_LINK_IN);
  mjr_create_container_linklist(newcntnr, MJR_LINK_OUT);

  /* Create name and md5 string for function */
  if (name)
    strncpy(newfunction->name, (char *) name, sizeof(newfunction->name)-1);
  else
    newfunction->name[0] = 0;  
  newfunction->first = NULL;
  if (md5)
    memcpy(newfunction->md5, md5, sizeof(newfunction->md5));
  else
    memset(newfunction->md5, 0, sizeof(newfunction->md5));

#if __DEBUG_CNTNR__
  fprintf(D_DESC,"[D] %s: create func %x/<%s> (%d) %s\n",
	  __FUNCTION__, vaddr, name, size, md5);
#endif

  mjr_register_container(ctx, newcntnr);  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}


/**
 * This function should be used only for debug
 * It is O(n) since this api doesn't know about hashes in the mjr context
 */
mjrcontainer_t		*mjr_get_container_by_vaddr(mjrcontext_t *ctx, elfsh_Addr vaddr, int type)
{
  mjrcontainer_t	*cur;
  u_int			idx;

  PROFILER_IN(__FILE__,__FUNCTION__,__LINE__);

  for (idx = 1, cur = ctx->reg_containers[idx]; cur; cur = ctx->reg_containers[++idx])
    if (cur->type == type && *(elfsh_Addr *) cur->data == vaddr)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cur);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);    
}


/* Debug output help function for containers */
void			mjr_container_dump(mjrcontext_t *ctx, int what)
{
  mjrcontainer_t	*cur;
  mjrfunc_t		*tf;
  u_int			idx;

  for (idx = 1, cur = ctx->reg_containers[idx]; cur; cur = ctx->reg_containers[++idx])
    {
      if (cur->type == what)
	{
	  if (cur->type == ASPECT_TYPE_FUNC)
	    {
	      tf = cur->data;
	      printf("FOUND: FUNC T:%d V:%x I:%d O:%d\n",
		     what, tf->vaddr, cur->inlinks->elmnbr, cur->outlinks->elmnbr);
	    }
	}
    }
}


/* Get the list of links for the desired direction (input or output) */
list_t		*mjr_link_get_by_direction(mjrcontainer_t *c, int dir)
{
  list_t	*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = NULL;
  if (dir == MJR_LINK_IN)
    ret = c->inlinks;
  else if (dir == MJR_LINK_OUT)
    ret = c->outlinks;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}








