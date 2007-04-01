/*
** libmjollnir/src/blocks.c
** 
** Authors : sk, mayhem, thorkill, strauss
** 
** Started : Thu May 29 20:39:14 2003 sk
** Updated : Fri Dec 15 01:09:47 2006 mayhem
**
** $Id: blocks.c,v 1.52 2007-04-01 23:33:16 may Exp $
**
*/
#include "libmjollnir.h"

/* Goto hash */
hash_t		goto_hash;

/**
 * This function moves dependency of blocks
 * which are continuation of src to dst
 * in specified direction and cleanups the old
 * in src
 */
int	mjr_block_relink_cond_always(mjrcontainer_t *src,
				      mjrcontainer_t *dst,
				      int direction)
{

  mjrlink_t *lnk;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  lnk = mjr_link_get_by_direction(src, direction);

#if __DEBUG_BLOCKS__
  fprintf(D_DESC,"[D] %s: src:%d dst:%d dir:%d\n",
	  __FUNCTION__, src->id, dst->id, direction);
#endif

  while(lnk)
    {
      if ((lnk->type == MJR_LINK_BLOCK_COND_ALWAYS) && (lnk->id != dst->id))
	{
	  // 1 - add same links to dst
	  mjr_container_add_link(dst,lnk->id, MJR_LINK_BLOCK_COND_ALWAYS, direction);
	  // 2 - and remove it from src
	  lnk->type = MJR_LINK_DELETE;
	}
      lnk = lnk->next;
    }

  mjr_container_link_cleanup(src,direction);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}

int	mjr_blocks_link_call(mjrcontext_t *ctxt,
			     elfsh_Addr src,
			     elfsh_Addr dst,
			     elfsh_Addr ret)
{
  mjrcontainer_t	*csrc,*cdst,*cret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_BLOCKS__
  fprintf(D_DESC,"[D] %s: linking %x CALL %x RET %x\n",
	  __FUNCTION__,
	  src,dst,ret);
#endif

  csrc = mjr_block_get_by_vaddr(ctxt, src, 1);

  assert(csrc != NULL);

  if (!(cdst = mjr_split_block(ctxt,dst)))
    PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
		 "Could not split the dst",0);

  if (!(cret = mjr_split_block(ctxt,ret)))
    PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
		 "Could not split the ret",0);
  
  mjr_container_add_link(csrc, cdst->id, MJR_LINK_FUNC_CALL, MJR_LINK_OUT);
  mjr_container_add_link(cdst, csrc->id, MJR_LINK_FUNC_CALL, MJR_LINK_IN);

  mjr_container_add_link(cdst, cret->id, MJR_LINK_FUNC_RET, MJR_LINK_OUT);
  mjr_container_add_link(cret, cdst->id, MJR_LINK_FUNC_RET, MJR_LINK_IN);

  mjr_block_relink_cond_always(csrc,cret,MJR_LINK_OUT);
  mjr_block_relink_cond_always(cret,csrc,MJR_LINK_IN);

#if __DEBUG_BLOCKS__
  mjr_block_dump(csrc);
  mjr_block_dump(cdst);
  mjr_block_dump(cret);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}

int	mjr_blocks_link_jmp(mjrcontext_t *ctxt,
			     elfsh_Addr src,
			     elfsh_Addr dst,
			     elfsh_Addr ret)
{
  mjrcontainer_t	*csrc,*cdst,*cret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_BLOCKS__
  fprintf(D_DESC,"[D] %s: linking JMP %x TRUE %x FALSE %x\n",
	  __FUNCTION__,
	  src,dst,ret);
#endif

  if (!(cdst = mjr_split_block(ctxt,dst)))
    PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
		 "Could not split the dst",0);

  csrc = mjr_block_get_by_vaddr(ctxt, src, 1);

  assert(csrc != NULL);

  cret = NULL;

  if (ret)
      if (!(cret = mjr_split_block(ctxt,ret)))
	PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
		     "Could not split the ret",0);
  
  mjr_container_add_link(csrc, cdst->id, MJR_LINK_BLOCK_COND_TRUE, MJR_LINK_OUT);
  mjr_container_add_link(cdst, csrc->id, MJR_LINK_BLOCK_COND_TRUE, MJR_LINK_IN);

  if (cret)
    {
      mjr_container_add_link(csrc, cret->id, MJR_LINK_BLOCK_COND_FALSE, MJR_LINK_OUT);
      mjr_container_add_link(cret, csrc->id, MJR_LINK_BLOCK_COND_FALSE, MJR_LINK_IN);

      mjr_block_relink_cond_always(csrc,cret,MJR_LINK_OUT);
      mjr_block_relink_cond_always(cret,csrc,MJR_LINK_IN);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}

mjrcontainer_t	*mjr_split_block(mjrcontext_t *ctxt,
				 elfsh_Addr dst)
{
  mjrcontainer_t	*tmpdst,*dstend;
  mjrblock_t		*blkdst;
  int			new_size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  tmpdst = mjr_block_get_by_vaddr(ctxt, dst, 1);

  if (!tmpdst)
    {
      tmpdst = mjr_create_block_container(0, dst, 1);
      hash_add(&ctxt->blkhash, _vaddr2str(dst), tmpdst);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (tmpdst));
    }

  blkdst = (mjrblock_t *)tmpdst->data;

#if __DEBUG_BLOCKS__
  fprintf(D_DESC,"[D] %s: wanted dst:%x got:%x\n",
	  __FUNCTION__,dst,blkdst->vaddr);
#endif

  if (blkdst->vaddr != dst)
    {
      new_size		= blkdst->size - (dst - blkdst->vaddr);
      blkdst->size	-= new_size;

      assert(new_size > 0);
      assert(blkdst->size > 0);

      dstend		= mjr_create_block_container(0, dst, new_size);
      hash_add(&ctxt->blkhash, _vaddr2str(dst), dstend);

      mjr_container_add_link(tmpdst, dstend->id, MJR_LINK_BLOCK_COND_ALWAYS, MJR_LINK_OUT);
      mjr_container_add_link(dstend, tmpdst->id, MJR_LINK_BLOCK_COND_ALWAYS, MJR_LINK_IN);
    } 
  else 
    {
      dstend = tmpdst;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (dstend));
}

/**
 * Retreive control flow section content if any 
 *
 * returns the number of blocks (0 probably means something is wrong)
 */
int	mjr_blocks_get(mjrcontext_t *ctxt)
{
  elfshsect_t		*sect;
  int			cnt;
	
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
 /* Parse arguments, load binary and resolve symbol */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_EDFMT_BLOCKS, 0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No control flow section : use analyse command", 0);

  /* Return or retreive information */
	cnt = hash_size(&ctxt->blkhash);
  if (cnt != 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);

  cnt = mjr_blocks_load(ctxt);

  if (cnt != 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Flow analysis failed", cnt);
}


/**
 * Create the control flow graph using the information stored in .elfsh.control 
 *
 * returns the number of blocks (0 probably means something is wrong)
 */
int   mjr_blocks_load(mjrcontext_t *ctxt)
{
  int                   index, findex, tmptype, cnt, done, flowdone, tmpnbr;
  elfshsect_t           *sect, *flowsect;
  mjrcontainer_t	*curcntnr;
  mjrblock_t		*curblock;
  unsigned int		blocnbr, tmpid, off;
  char			name[20];
  char			**keys;
	
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_EDFMT_BLOCKS, 0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No control flow section : use analyse command", 0);
  if (sect->shdr->sh_size % (sizeof(mjrblock_t) + sizeof(mjrcontainer_t)))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Corrupted control flow section : modulo-test failed", 0);

  flowsect = elfsh_get_section_by_name(ctxt->obj,
				       ELFSH_SECTION_NAME_EDFMT_BCONTROL,
				       0, 0, 0);
  
  if (!flowsect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "No block flow section : use analyse command", 0);
  
  /* If the function was already called, return its result */
  cnt = hash_size(&ctxt->blkhash);
  if (cnt != 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);
  
  /* First pass : Iterate on the control flow section to find blocks */
  /* We do not create new blocks but use the data from the section */
  blocnbr = sect->shdr->sh_size / (sizeof(mjrcontainer_t) + sizeof(mjrblock_t)) ;
  done = 0;
  for (index = 0; index < blocnbr; index++)
    {
      mjrcontainer_t *tmpcntnr;
      mjrblock_t *tmpblk;

      // here you can see that the format ondisk is the exact same format that
      // we manipulate in this lib. So its very practical to reload/store the
      // info ondisk.
      curcntnr = (mjrcontainer_t *)((char *)sect->data + done);
      done += sizeof(mjrcontainer_t);

      curblock = (mjrblock_t *)((char *)sect->data + done);
      done += sizeof(mjrblock_t);

      XALLOC(__FILE__, __FUNCTION__, __LINE__, tmpblk, sizeof(mjrfunc_t), NULL);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, tmpcntnr, sizeof(mjrcontainer_t), NULL);

      memcpy(tmpcntnr, curcntnr, sizeof(mjrcontainer_t));
      memcpy(tmpblk, curblock, sizeof(mjrblock_t));

      tmpcntnr->data = tmpblk;

      snprintf(name, sizeof(name), AFMT, tmpblk->vaddr);
      
#if __DEBUG_BLOCKS__
      fprintf(D_DESC,"[__DEBUG__] %s: add new block name:%s\n",__FUNCTION__,name);
#endif
      mjr_register_container_id (tmpcntnr);
      hash_add(&ctxt->blkhash, (char *) _vaddr2str(tmpblk->vaddr), tmpcntnr);
    }

  keys = hash_get_keys(&ctxt->blkhash, &done);
  for (index = 0; index < done; index++)
    {
      curcntnr = hash_get(&ctxt->blkhash, keys[index]);

      off = (u_int) curcntnr->input;
      curcntnr->input = NULL;

      /* fill the flow list */

      /* prevent endless loop */
      tmpnbr = curcntnr->in_nbr;
      curcntnr->in_nbr = 0;
      flowdone = 0;
      for (findex=0; findex < tmpnbr; findex++)
	{	 
	  tmpid = *(unsigned int *)((char *)flowsect->data + off + flowdone);
	  flowdone += sizeof(unsigned int);
	  tmptype = *(int *)((char *)flowsect->data + off + flowdone);
	  flowdone += sizeof(int);

	  fprintf(D_DESC," [D] resotore parent: (%d/%d) sid:%u did:%u type:%u\n", 
		  findex,tmpnbr,
		  curcntnr->id,tmpid,tmptype);
	  
	  /* this function increments in_nbr */
	  mjr_container_add_link(curcntnr, tmpid, tmptype, MJR_LINK_IN);
	}

      off = (u_int)curcntnr->output;
      curcntnr->output = NULL;

      /* prevent endless loop */
      tmpnbr = curcntnr->out_nbr;
      curcntnr->out_nbr = 0;
      for (findex=0; findex < tmpnbr; findex++)
	{
	  tmpid = *(unsigned int *)((char *)flowsect->data + off + flowdone);
	  flowdone += sizeof(unsigned int);
	  tmptype = *(int *)((char *)flowsect->data + off + flowdone);
	  flowdone += sizeof(int);

	  fprintf(D_DESC," [D] resotore child: (%d/%d) sid:%u did:%u type:%u\n", 
		  findex,tmpnbr,
		  curcntnr->id,tmpid,tmptype);
	  
	  /* out_nbr++ */
	  mjr_container_add_link(curcntnr, tmpid, tmptype, MJR_LINK_OUT);
	}
      mjr_block_dump(curcntnr);
    }

  /* FIXME: prevent double analysis */
  ctxt->analysed = 1;

  cnt = hash_size(&ctxt->blkhash);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);
}


/**
 * Save and prepare a buffer which will be saved into the elfshobj.
 *
 * @param c a block container containing links to be stores
 * @param type 0 for inout/1 for output
 * @param buf buffer which contains the data
 * @return an offset where the data has been saved
 */

u_int	 mjr_block_flow_save(mjrcontainer_t *c, u_int type, mjrbuf_t *buf)
{
  u_int curOff,nbr;
  mjrlink_t *cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (type == 0)
  {
    cur = c->input;
    nbr = c->in_nbr;
  }
  else if (type == 1)
  {
    cur = c->output;
    nbr = c->out_nbr;
  }

  if (!cur)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (!buf->data)
  {
    buf->allocated = getpagesize();
    XALLOC(__FILE__, __FUNCTION__, __LINE__, buf->data, buf->allocated, -1);
    buf->maxlen = 0;
  }
  else if (buf->allocated  < (buf->maxlen + (sizeof(unsigned int) * nbr * 2)))
  {
    buf->allocated += getpagesize();
    XREALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     buf->data, buf->data, buf->allocated, -1);
  }

#if __DEBUG_BLOCKS__
  fprintf(D_DESC,"[D] %s: allocated: %d needed: %d endsize: %d pagesize: %d\n",
	  __FUNCTION__, buf->allocated,
	  (sizeof(unsigned int) * nbr * 2),
	  buf->maxlen + (sizeof(unsigned int) * nbr * 2),
	  getpagesize());
#endif

  while (cur)
  {
    memcpy(buf->data + buf->maxlen, (char *)&cur->id , sizeof(unsigned int));
    buf->maxlen += sizeof(unsigned int);
    memcpy(buf->data + buf->maxlen, (char *)&cur->type , sizeof(int));
    buf->maxlen += sizeof(int);
    cur = cur->next;
  }
  
  buf->block_counter++;
  curOff = buf->maxlen;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (curOff));
}
/**
 * Create the block information to be saved in file 
 */
int	mjr_block_save(mjrcontainer_t *cur, mjrbuf_t *buf)
{
  char			buffer[BUFSIZ];
  elfsh_Sym		bsym;
  elfsh_Sym		*sym;
  mjrblock_t		*curblock;
  mjrcontainer_t	*curcntnr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* At this points, no new block allocation should be done */
  curblock = cur->data;
  snprintf(buffer, sizeof(buffer), "block_%lX", (unsigned long) curblock->vaddr);

  sym = elfsh_get_symbol_by_name(buf->obj, buffer);
  if (sym)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  
#if __DEBUG_BLOCKS__
  fprintf(D_DESC," [*] Saving block at addr %s \n", buffer);
#endif

  /* Else insert the block in the global buffer for the .control section */
  if (!buf->data) 
  {
    buf->allocated = getpagesize();
    XALLOC(__FILE__, __FUNCTION__, __LINE__, buf->data, buf->allocated, -1);
    buf->maxlen = 0;
  }

  else if (buf->allocated  < (buf->maxlen + sizeof(mjrblock_t) + sizeof(mjrcontainer_t)))
  {
    buf->allocated += getpagesize();
    XREALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     buf->data, buf->data, buf->allocated, -1);
  }
  
  curcntnr = (mjrcontainer_t *) ((char *) buf->data + buf->maxlen);
  curblock = (mjrblock_t *) ((char *) buf->data + buf->maxlen + 
			     sizeof(mjrcontainer_t));


  memcpy(curcntnr, cur, sizeof(mjrcontainer_t));
  memcpy(curblock, cur->data, sizeof(mjrblock_t));
  
  /* Then we create the symbol for the bloc and returns */
  bsym = elfsh_create_symbol(curblock->vaddr, curblock->size, STT_BLOCK, 0, 0, 0);
  elfsh_insert_symbol(buf->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, buffer);
  buf->maxlen += sizeof(mjrcontainer_t);
  buf->maxlen += sizeof(mjrblock_t);
  buf->block_counter++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Store the blocks inside the .control section using the file representation 
 */
// This loops on all block, and call blocks_save on each bloc
int		mjr_blocks_store(mjrcontext_t *ctxt) 
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  mjrbuf_t		buf, cfbuf;
  mjrcontainer_t	*blkcntnr, *funcntnr;
  mjrblock_t		*block;
  mjrfunc_t		*func;
  int			err;
  char			**keys;
  int			keynbr;
  int			index;
  char			funcname[50];
  u_int			flow_off_in, flow_off_out;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Remove previous control section if any */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_EDFMT_BLOCKS, 0, 0, 0);
  if (sect)
    elfsh_remove_section(ctxt->obj, ELFSH_SECTION_NAME_EDFMT_BLOCKS);
	
  /* Initialize data buffer */
  buf.allocated     = 0;
  buf.maxlen        = 0;
  buf.block_counter = 0;
  buf.data          = 0;
  buf.obj           = ctxt->obj;

  /* Initialize data buffer */
  cfbuf.allocated     = 0;
  cfbuf.maxlen        = 0;
  cfbuf.block_counter = 0;
  cfbuf.data          = 0;
  cfbuf.obj           = ctxt->obj;

  flow_off_in = flow_off_out = 0;

  /* Iteratively save all blocks */
  keys = hash_get_keys(&ctxt->blkhash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      mjrlink_t *tmpin,*tmpout;

      tmpin = tmpout = NULL;

      blkcntnr = hash_get(&ctxt->blkhash, keys[index]);

      /* Setting references to link information inside the file */
      tmpin = blkcntnr->input;
      blkcntnr->input = (mjrlink_t *) flow_off_in;
      tmpout = blkcntnr->output;
      blkcntnr->output = (mjrlink_t *) flow_off_out;

      mjr_block_save(blkcntnr, &buf);

      /* 
	 resotre old lists 
	 FIXME: we could retrieve the block once again
	 what do you think? /thorkill 200703
       */
      blkcntnr->input = tmpin;
      blkcntnr->output = tmpout;

#if __DEBUG_BLOCKS__
      fprintf(D_DESC, " [D] %s: IN  Offset: %d\n", __FUNCTION__, flow_off_in);
      fprintf(D_DESC, " [D] %s: OUT Offset: %d\n", __FUNCTION__, flow_off_out);
#endif

      /* Store block flow and get new offsets */
      flow_off_in = mjr_block_flow_save(blkcntnr, 0, &cfbuf);
      flow_off_out = mjr_block_flow_save(blkcntnr, 1, &cfbuf);

      block = (mjrblock_t *) blkcntnr->data;

      if (mjr_block_funcstart(blkcntnr))
	{
	  snprintf(funcname, sizeof(funcname), AFMT, block->vaddr);
	  funcntnr = hash_get(&ctxt->funchash, funcname);

	  /* Can happens rarely - should not be fatal */
	  if (func == NULL)
	    {
	      printf(" [*] Failed to find parent function at %s \n", funcname);
	      continue;
	    }
	  printf(" [*] Found block start for function %s \n", funcname);
	}
    }

  /* Create control section */
  sect = elfsh_create_section(ELFSH_SECTION_NAME_EDFMT_BLOCKS);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, buf.maxlen, 0, 0, 0, 0);

  printf(" [*] Saving .edfmt.blocks section of %u bytes \n", buf.maxlen);
  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, buf.data);

  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to save .edfmt.blocks section", -1);
  
  sect = elfsh_create_section(ELFSH_SECTION_NAME_EDFMT_BCONTROL);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, cfbuf.maxlen, 0, 0, 0, 0);

  printf(" [*] Saving .edfmt.bcontrol section of %u bytes\n", cfbuf.maxlen);

  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, cfbuf.data);

  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to save .edfmt.bcontrol section", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.block_counter);
}

/** 
 * Get a block by vaddr
 *
 * If mode = 0, return block only if vaddr is equal to block starting address
 * else return block if vaddr belong to block
 */
mjrcontainer_t	*mjr_block_get_by_vaddr(mjrcontext_t 	*ctxt, 
					elfsh_Addr   	vaddr, 
					int		mode)
{
  mjrcontainer_t	*ret;
  mjrblock_t		*tmpblock;
  char			**keys;
  int			index;
  int			size;

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
  
  if ((mode == 0) || ((mode == 1) && (ret)))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
  
  /* Parent match */
  keys = hash_get_keys(&ctxt->blkhash, &size);
  for (index = 0; index < size; index++)
    {
      ret = hash_get(&ctxt->blkhash, keys[index]);
      tmpblock = (mjrblock_t *) ret->data;

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

    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
}

int mjr_block_dump(mjrcontainer_t *c)
{
  mjrblock_t *blk,*tblk;
  mjrlink_t  *lnk;

  blk = (mjrblock_t *)c->data;
  fprintf(D_DESC,"[D] %s: vaddr:%x size:%d in container ID:%d OUT:%d IN:%d\n",
	  __FUNCTION__,
	  blk->vaddr, blk->size,
	  c->id,c->out_nbr,c->in_nbr);

  lnk = c->input;
  while(lnk)
    {
      tblk = (mjrblock_t *) mjr_lookup_container(lnk->id)->data;
      fprintf(D_DESC,"[D] %s: %x linked IN from vaddr:%x size:%d type:%d\n",
	      __FUNCTION__, blk->vaddr, tblk->vaddr, tblk->size, lnk->type);
      lnk = lnk->next;
    }

  lnk = c->output;
  while(lnk)
    {
      tblk = (mjrblock_t *) mjr_lookup_container(lnk->id)->data;
      fprintf(D_DESC,"[D] %s: %x linked OUT to vaddr:%x size:%d type:%d\n",
	      __FUNCTION__, blk->vaddr, tblk->vaddr, tblk->size, lnk->type);
      lnk = lnk->next;
    }

  return 0;
}
