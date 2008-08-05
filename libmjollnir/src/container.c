/**
 * 2001-2007 Devhell Labs, Rfdslabs, Asgardlabs
 *
 * @file container.c
 * @brief An API for generic containers data structures
 *
 */

#include "libmjollnir.h"

/**
 * @brief Initialize containers for context
 * @param ctx mjollnir context
 */
int	mjr_init_containers(mjrcontext_t *ctx)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, ctx->reg_containers, 
	 sizeof(container_t*) * ctx->cntnrs_size, -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Resize containers for given context
 * @param ctx mjollnir context
 */
int	mjr_resize_containers(mjrcontext_t *ctx, unsigned int resize)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ctx->cntnrs_size += (resize ? resize + 1 : MJR_CNTNRS_INCREMENT);
  XREALLOC(__FILE__, __FUNCTION__, __LINE__, 
	   ctx->reg_containers, ctx->reg_containers,
	   sizeof(container_t*) * ctx->cntnrs_size, -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Register new container for given context
 * @param ctx mjollnir context
 * @param cntnr container
 */
unsigned int mjr_register_container(mjrcontext_t *ctx, container_t *cntnr)
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
	  __FUNCTION__, *(eresi_Addr *) cntnr->data, cntnr->id, cntnr->type);
#endif

  return ctx->next_id++;
}

/**
 * Register container in context by id
 * @param ctx mjollnir context
 */
unsigned int mjr_register_container_id(mjrcontext_t *ctx, container_t *cntnr)
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
	  __FUNCTION__, *(eresi_Addr *) cntnr->data, cntnr->id);
#endif

  return cntnr->id;
}

/**
 * @brief Unregister container by id
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
container_t *mjr_lookup_container(mjrcontext_t *ctx, unsigned int id)
{
  return ctx->reg_containers[id];
}


/**
 * Add a generic link (input or output) to a container
 */
mjrlink_t	*mjr_container_add_link(mjrcontext_t	*ctx,
					container_t	*cntnr,
					unsigned int	id,
					u_char		link_type,
					u_char		link_scope,
					int		link_direction)
{
  list_t	*linklist;
  listent_t	*listent;
  listent_t	*savednext;
  mjrlink_t	*link;
  container_t	*cnt;
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
	      fprintf(D_DESC,"[D] %s: removing old unconditional link id:%d for condlink type %d\n",
		      __FUNCTION__, id, link_type);
#endif

	      elist_del(linklist, listent->key);
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
  link->scope = link_scope;
  snprintf(linkname, sizeof(linkname), "%u_%u", cntnr->id, id);
  elist_add(linklist, strdup(linkname), link);
  hash_add(&ctx->linkhash, strdup(linkname), link);
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

  blk_elem = (mjrblock_t *)((container_t *)elem)->data;
  blk_match = (mjrblock_t *)((container_t *)match)->data;

  return (blk_match->vaddr - blk_elem->vaddr);
}




/**
 * @brief Creates a block container
 */
container_t	*mjr_create_block_container(mjrcontext_t	*ctx,
					    u_int		symoff,
					    eresi_Addr		vaddr,
					    u_int		size,
					    u_char		seen)
{
  mjrblock_t 	*newblock;
  container_t	*newcntnr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_CNTNR__
  newcntnr = mjr_get_container_by_vaddr(ctx, vaddr, ASPECT_TYPE_BLOC);
  if (newcntnr)
    {
      fprintf(D_DESC,"[D] %s: block container %x id:%d is there ALREADY\n",
	      __FUNCTION__, vaddr, newcntnr->id);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
    }
#endif

  newblock         = alloca(sizeof(mjrblock_t));
  bzero(newblock, sizeof(mjrblock_t));
  newblock->symoff = symoff;
  newblock->vaddr  = vaddr;
  newblock->size   = size;
  newblock->seen   = seen;
  newcntnr         = container_create(ASPECT_TYPE_BLOC, newblock, NULL, NULL);
  if (!newcntnr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to create block container", NULL);  

#if __DEBUG_CNTNR__
  fprintf(D_DESC, "[D] %s: create block addr " XFMT " (sz %d)\n", __FUNCTION__, vaddr, size);
#endif

  mjr_register_container(ctx, newcntnr);

  /* fill the btree */
  btree_insert_sort(&ctx->block_btree, match_block, newcntnr);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}


/**
 * Creates function container
 */
container_t	*mjr_create_function_container(mjrcontext_t	*ctx,
					       eresi_Addr	vaddr,
					       u_int		size,
					       char		*name,
					       mjrblock_t	*first,
					       char		*md5)
{
  mjrfunc_t	*newfunction;
  container_t	*newcntnr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
#if __DEBUG_CNTNR__
  newcntnr = mjr_get_container_by_vaddr(ctx, vaddr, ASPECT_TYPE_FUNC);
  if (newcntnr)
    {
      fprintf(D_DESC,"[D] %s: func container addr %x id:%d is there ALREADY\n",
	      __FUNCTION__, vaddr, newcntnr->id);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
    }
#endif

  /* Allocate the new container and its links lists */
  newfunction = alloca(sizeof(mjrfunc_t));
  bzero(newfunction, sizeof(mjrfunc_t));
  newfunction->vaddr = vaddr;
  newfunction->size  = size;

  /* Create name and md5 string for function */
  if (name)
    strncpy(newfunction->name, (char *) name, sizeof(newfunction->name) - 1);
  if (md5)
    memcpy(newfunction->md5, md5, sizeof(newfunction->md5));

#if __DEBUG_CNTNR__
  fprintf(D_DESC,"[D] %s: create func addr: %x name: <%s> (sz %d) (md5 %s)\n",
	  __FUNCTION__, vaddr, name, size, md5);
#endif

  newcntnr = container_create(ASPECT_TYPE_FUNC, newfunction, NULL, NULL);
  if (!newcntnr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to create function container", NULL);  
  mjr_register_container(ctx, newcntnr);  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}


/**
 * @brief This function should be used only for debug
 * It is O(n) since this api doesn't know about hashes in the mjr context
 */
container_t	*mjr_get_container_by_vaddr(mjrcontext_t *ctx, eresi_Addr vaddr, int type)
{
  container_t	*cur;
  u_int		idx;

  PROFILER_IN(__FILE__,__FUNCTION__,__LINE__);

  for (idx = 1, cur = ctx->reg_containers[idx]; cur && idx < ctx->next_id; 
       cur = ctx->reg_containers[++idx])
    if (cur->type == type && *(eresi_Addr *) cur->data == vaddr)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cur);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);    
}


/* Debug output help function for containers */
void		mjr_container_dump(mjrcontext_t *ctx, int what)
{
  container_t	*cur;
  mjrfunc_t	*tf;
  u_int		idx;
  
  for (idx = 1, cur = ctx->reg_containers[idx]; cur; cur = ctx->reg_containers[++idx])
    {
      if (cur->type == what)
	{
	  if (cur->type == ASPECT_TYPE_FUNC)
	    {
	      tf = cur->data;
	      printf("FOUND: FUNC T:%d V:" AFMT " I:%d O:%d\n",
		     what, tf->vaddr, cur->inlinks->elmnbr, cur->outlinks->elmnbr);
	    }
	  else if (cur->type == ASPECT_TYPE_BLOC)
	    {
	      mjr_block_dump(ctx, cur);
	    }
	}
    }
}


/**
 * @brief Get the list of links for the desired direction (input or output) 
 */
list_t		*mjr_link_get_by_direction(container_t *c, int dir)
{
  list_t	*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = NULL;
  if (dir == CONTAINER_LINK_IN)
    ret = c->inlinks;
  else if (dir == CONTAINER_LINK_OUT)
    ret = c->outlinks;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}








