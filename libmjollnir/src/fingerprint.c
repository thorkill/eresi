/*
** fingerprint.c in libmjollnir for elfsh
** 
** Functions for fingerprinting inside libmjollnir
**
** Started : Mon Jan 02 01:18:14 2007 mayhem
**
** $Id: fingerprint.c,v 1.13 2007-04-09 15:18:05 thor Exp $
**
*/
#include "libmjollnir.h"

/**
 * Say if a block is the start of a function or not 
 */
int		 mjr_block_funcstart(mjrcontainer_t *cntnr) 
{
  mjrlink_t	 *cur;
	mjrblock_t *blk;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);


  if (cntnr && cntnr->type == MJR_CNTNR_BLOCK) 
    {
      blk = cntnr->data;
      
#if __DEBUG_BLOCK__
      fprintf(D_DESC,"[D] mjr_block_funcstart: blk:"XFMT"\n",blk->vaddr);
#endif
      
      for (cur = cntnr->input; cur; cur = cur->next) 
	{
#if __DEBUG_BLOCK__
	  fprintf(D_DESC,"[D] mjr_block_funcstart: clr:"XFMT"/%d\n",
		  cur->vaddr,cur->type);
#endif
	  
	  if (cur->type == MJR_LINK_FUNC_RET)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Depth First Search: distance fingerprinting algorithm
 * @param c mjollnir context
 * @param start mjollnir container
 * @param type 0 for forward-fingerprinting, 1 for backwards
 * @param curd current depth, Mind: minimum depth to search for, Maxd: Maximum depth
 * @param weight : allow finer fingerprint by counting hits (should start with 0)
 * @param mind min depth to match
 * @param maxd max depth to match
 * @param fprint : any intrabloc fingerprinting function that you like
 */
int	mjr_fprint		(mjrcontext_t 	*c,
				 mjrcontainer_t	*start,
				 int		type,
				 int		weight,
				 int		curd, // cur depth
				 int		mind, // min depth to match
				 int		maxd, // max depth to match
				 int	(*fprint)(mjrcontainer_t *))
{
  mjrlink_t	*link;

  if (type == 0)
    link = start->output;
  else if (type == 1)
    link = start->input;
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

  /* Terminal recursion except when both true and false are != NULL */
  for (; link; link = link->next) 
    {
      mjrcontainer_t *tmp = mjr_lookup_container(c, link->id);
      weight += mjr_fprint(c, tmp, type, weight, curd, mind, maxd, fprint);
    }
  
  return weight;
}
