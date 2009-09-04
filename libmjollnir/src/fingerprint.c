/**
 * @file fingerprint.c
 * @ingroup libmjollnir
 * 
 * @brief Functions for fingerprinting inside libmjollnir
 *
 * Started : Mon Jan 02 01:18:14 2007 jfv
 *
 */

#include "libmjollnir.h"


/**
 * @brief Say if a block is the start of a function or not 
 */
int		 mjr_block_funcstart(container_t *cntnr) 
{
  listent_t	 *cur;
  mjrlink_t	 *curlink;
  mjrblock_t	 *blk;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!cntnr || cntnr->type != ASPECT_TYPE_BLOC)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  blk = cntnr->data;
  
#if __DEBUG_BLOCK__
  fprintf(D_DESC,"[D] mjr_block_funcstart: blk:"XFMT"\n", blk->vaddr);
#endif
  
  for (cur = cntnr->inlinks->head; cur; cur = cur->next) 
    {
      curlink = (mjrlink_t *) cur->data;
      
#if __DEBUG_BLOCK__
      fprintf(D_DESC,"[D] mjr_block_funcstart: clr:"XFMT"/%d\n",
	      curlink->vaddr, curlink->type);
#endif
      
      if (curlink->type == MJR_LINK_FUNC_RET)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Depth First Search: distance fingerprinting algorithm
 * @param c mjollnir context
 * @param start mjollnir container
 * @param type 0 for forward-fingerprinting, 1 for backwards
 * @param curd current depth, Mind: minimum depth to search for, Maxd: Maximum depth
 * @param weight : allow finer fingerprint by counting hits (should start with 0)
 * @param mind min depth to match
 * @param maxd max depth to match
 * @param fprint : any intrabloc fingerprinting function that you like
 */
int		mjr_fingerprint(mjrcontext_t 	*c,
				container_t	*start,
				int		type,
				int		weight,
				int		curd, // cur depth
				int		mind, // min depth to match
				int		maxd, // max depth to match
				int		(*fprint)(container_t *))
{
  container_t *tmp;
  list_t	*listlink;
  listent_t	*cur;
  mjrlink_t	*curlink;

  if (type == CONTAINER_LINK_OUT)
    listlink = start->outlinks;
  else if (type == CONTAINER_LINK_IN)
    listlink = start->inlinks;
  else
    return 0;
  
  /* We are not yet in the depth range of fingerprint, just recurse */
  if (curd < mind)
    curd++;
  
  /* We are in the range. Fingerprint and recurse */
  else if (curd >= mind && maxd >= curd)
    {
      weight += fprint(start);
      curd++;
    }

  /* We passed the max distance. Stop recursing */
  else
    return (weight);

  /* Now recursing ! */
  for (cur = listlink->head; cur; cur = cur->next) 
    {
      curlink = (mjrlink_t *) cur->data;
      tmp = mjr_lookup_container(c, curlink->id);
      weight += mjr_fingerprint(c, tmp, type, weight, curd, mind, maxd, fprint);
    }
  
  return weight;
}
