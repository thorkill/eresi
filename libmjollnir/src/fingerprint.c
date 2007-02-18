/*
** fingerprint.c in libmjollnir for elfsh
** 
** Functions for fingerprinting inside libmjollnir
**
** Started : Mon Jan 02 01:18:14 2007 mayhem
*/
#include "libmjollnir.h"



/* Say if a block is the start of a function or not */
int		 mjr_block_funcstart(mjrblock_t *blk) 
{
  mjrcaller_t	 *cur;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);


  if (blk) {
    //  fprintf(D_DESC,"[D] mjr_block_funcstart: blk:"XFMT"\n",blk->vaddr);
    for (cur = blk->caller; cur; cur = cur->next) {
      //     fprintf(D_DESC,"[D] mjr_block_funcstart: clr:"XFMT"/%d\n",
      //  cur->vaddr,cur->type);
      if (cur->type == CALLER_CALL)
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
    }
  }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Depth First Search : forward distance fingerprinting algorithm */
/* Curd: current depth, Mind: minimum depth to search for, Maxd: Maximum depth */
/* Weight : allow finer fingerprint by counting hits (should start with 0) */
/* Fprint : any intrabloc fingerprinting function that you like */
int		mjr_fprint_fwd(mjrcontext_t *c,
			       mjrblock_t   *start, 
			       int	    weight, 
			       int	    curd, // cur depth
			       int	    mind, // min depth to match
			       int	    maxd, // max depth to match
			       int	    (*fprint)(mjrblock_t *))
{
  mjrblock_t	*true;
  mjrblock_t	*false;

  /* Look children blocks */
  if (start->true)
    true = mjr_block_get_by_vaddr(c, start->true, 0);
  if (start->false)
    false = mjr_block_get_by_vaddr(c, start->false, 0);

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
  return (true && false ? 
	  mjr_fprint_fwd(c, true, weight, curd, mind, maxd, fprint) + 
	  mjr_fprint_fwd(c, false, weight, curd, mind, maxd, fprint) : 
	  true  ? mjr_fprint_fwd(c, true, weight, curd, mind, maxd, fprint)  :
	  false ? mjr_fprint_fwd(c, false, weight, curd, mind, maxd, fprint) :
	  weight);
}





/* Reverse Depth First Search : backward distance fingerprinting algorithm */
/* Curd: current depth, Mind: minimum depth to search for, Maxd: Maximum depth */
/* Weight : allow finer fingerprint by counting hits (should start with 0) */
/* Fprint : any intrabloc fingerprinting function that you like */
int		mjr_fprint_bwd(mjrcontext_t *c,
			       mjrblock_t   *start, 
			       int	    weight,
			       int	    curd,
			       int	    mind,
			       int	    maxd,
			       int	    (*fprint)(mjrblock_t *))
{
  mjrblock_t	*parent;
  mjrcaller_t	*caller;

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

  /* Non terminal recursion here .. */
  for (caller = start->caller; caller; caller = caller->next)
    {
      parent = mjr_block_get_by_vaddr(c, caller->vaddr, 0);
      weight += mjr_fprint_bwd(c, parent, weight, curd, mind, maxd, fprint);
    }
  return (weight);
}
