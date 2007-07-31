/*
** libmjollnir/src/blocks.c
** 
** Authors : sk, jfv, thorkill, strauss
** 
** Started : Thu May 29 20:39:14 2003 sk
** Updated : Fri Dec 15 01:09:47 2006 jfv
**
** $Id: blocks.c,v 1.66 2007-07-31 14:41:33 pouik Exp $
**
*/
#include "libmjollnir.h"

/* Goto hash */
hash_t		goto_hash;


/**
 * Retreive control flow section content if any 
 *
 * returns the number of blocks (0 probably means something is wrong)
 */
int			mjr_blocks_get(mjrcontext_t *ctxt)
{
  elfshsect_t		*sect;
  int			cnt;
	
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
 /* Parse arguments, load binary and resolve symbol */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_EDFMT_BLOCKS, 0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No control flow section : use analyse command", 0);

  /* Retreive information */
  cnt = hash_size(&ctxt->blkhash);
  if (cnt != 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);
  cnt = mjr_flow_load(ctxt, ASPECT_TYPE_BLOC);

  /* Return result */
  if (cnt != 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Flow analysis failed", -1);
}




/*
 * Get a block by vaddr
 *
 * If mode = 0, return block only if vaddr is equal to block starting address
 * else return block if vaddr belong to block
 */
mjrcontainer_t		*mjr_block_get_by_vaddr(mjrcontext_t 	*ctxt, 
						elfsh_Addr   	vaddr, 
						int		mode)
{
  mjrcontainer_t	*ret;
  btree_t		*retbtree;
  mjrblock_t		*tmpblock;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!ctxt)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "missing context", (NULL));

#if __DEBUG_BLOCKS__
  fprintf(D_DESC,"[D] %s: get %x (%d)\n",
	  __FUNCTION__, vaddr, mode);
#endif
  /* Exact match */
  ret = hash_get(&ctxt->blkhash, _vaddr2str(vaddr));

#if __DEBUG_BLK_LOOKUP__
  if (ret)
    fprintf(D_DESC,"[D] %s: found %x in hash\n",
	    __FUNCTION__, vaddr);
#endif
  
  if ((mode == MJR_BLOCK_GET_STRICT) || ((mode == MJR_BLOCK_GET_FUZZY) && (ret)))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
  
  retbtree = ctxt->block_btree;
  while(retbtree)
    {

      /* Parent match */
      ret = (mjrcontainer_t *)retbtree->elem;
      tmpblock = (mjrblock_t *)ret->data;

#if __DEBUG_BLK_LOOKUP__      
      fprintf(D_DESC,"[D] %s: checking block: %x:%x s:%d id:%d\n",
	      __FUNCTION__,
	      tmpblock->vaddr,
	      tmpblock->vaddr+tmpblock->size,
	      tmpblock->size,
	      ret->id);
#endif

      if ((tmpblock->vaddr <= vaddr) && (vaddr <= tmpblock->vaddr + tmpblock->size))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));

      if (tmpblock->vaddr < vaddr)
	retbtree = retbtree->right;
      else
	retbtree = retbtree->left;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
}




/* Simple debug function for block containers */
int		mjr_block_dump(mjrcontext_t* ctxt, mjrcontainer_t *c)
{
  mjrblock_t	*blk;
  mjrblock_t	*tblk;
  listent_t	*llnk;
  mjrlink_t	*lnk;

  blk = (mjrblock_t *) c->data;
  fprintf(D_DESC,"[D] %s: vaddr:%x size:%d in container ID:%d OUT:%d IN:%d\n",
	  __FUNCTION__,
	  blk->vaddr, blk->size,
	  c->id, c->outlinks->elmnbr, c->inlinks->elmnbr);

  for (llnk = c->inlinks->head; llnk; llnk = llnk->next)
    {
      lnk = (mjrlink_t *) llnk->data;
      tblk = (mjrblock_t *) mjr_lookup_container(ctxt, lnk->id)->data;
      fprintf(D_DESC,"[D] %s: %x linked IN from vaddr:%x size:%d type:%d\n",
	      __FUNCTION__, blk->vaddr, tblk->vaddr, tblk->size, lnk->type);
    }
  for (llnk = c->outlinks->head; llnk; llnk = llnk->next)
    {
      lnk = (mjrlink_t *) llnk->data;
      tblk = (mjrblock_t *) mjr_lookup_container(ctxt, lnk->id)->data;
      fprintf(D_DESC,"[D] %s: %x linked OUT to vaddr:%x size:%d type:%d\n",
	      __FUNCTION__, blk->vaddr, tblk->vaddr, tblk->size, lnk->type);
    }
  return 0;
}
