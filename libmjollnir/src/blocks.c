/*
** libmjollnir/src/blocks.c
** 
** Authors : sk, mayhem, thorkill
** 
** Started : Thu May 29 20:39:14 2003 sk
** Updated : Fri Dec 15 01:09:47 2006 mayhem
**
** $Id: blocks.c,v 1.47 2007-03-18 23:11:03 thor Exp $
**
*/
#include "libmjollnir.h"

/* Goto hash */
hash_t		goto_hash;


/**
 * Make the link between the new block and the current block.
 * Split existing blocks if necessary.
 * @param ctxt mjorllnir context strucutre
 * @param ins asm instruction
 * @param vaddr source address
 * @param dest destination address
 */
int	mjr_block_point(mjrcontext_t	*ctxt,
			asm_instr      	*ins,
			elfsh_Addr     	vaddr,
			elfsh_Addr     	dest)
{
  mjrcontainer_t	*dst, *dst_end, *tmpcntnr;
  mjrblock_t		*dstblk;
  mjrlink_t		*dst_true, *dst_false;
  int			new_size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  dst     = mjr_block_get_by_vaddr(ctxt, dest, 1);

  if (dst)
    dstblk = (mjrblock_t *) dst->data;

  /* Just create a new target block */
  if (!dst) 
  {
    dst = mjr_create_block_container(0, dest, 1);
    hash_add(&ctxt->blkhash, _vaddr2str(dest), dst);
  } 

  /* Split block */
  else if (dstblk->vaddr != dest) 
  {
    new_size        = dstblk->size - (dest - dstblk->vaddr);
    dstblk->size    -= new_size;
    dst_end         = mjr_create_block_container(0, dest, new_size);
    hash_add(&ctxt->blkhash, _vaddr2str(dest), dst_end);

    dst_true = mjr_get_link_of_type(dst->output, MJR_LINK_BLOCK_COND_TRUE);
    dst_false = mjr_get_link_of_type(dst->output, MJR_LINK_BLOCK_COND_FALSE);
    if (dst_true)
      mjr_container_add_link(dst_end, dst_true->id, MJR_LINK_BLOCK_COND_TRUE, MJR_LINK_OUT);
    if (dst_false)
      mjr_container_add_link(dst_end, dst_false->id, MJR_LINK_BLOCK_COND_FALSE, MJR_LINK_OUT);
    
    tmpcntnr = mjr_create_block_container(0, dest, 0);
    if (!dst_true)
      mjr_container_add_link(ctxt->curblock, tmpcntnr->id, MJR_LINK_BLOCK_COND_TRUE, MJR_LINK_OUT);
    else
      dst_true->id = tmpcntnr->id;
    
    tmpcntnr = mjr_create_block_container(0, 0, 0);
    if (!dst_false)
      mjr_container_add_link(ctxt->curblock, tmpcntnr->id, MJR_LINK_BLOCK_COND_FALSE,	MJR_LINK_OUT);
    else
      dst_false->id = tmpcntnr->id;
    
    dst             = dst_end;
  }
  
  tmpcntnr = mjr_block_get_by_vaddr(ctxt, vaddr, 0);
  if (!tmpcntnr)
    tmpcntnr = mjr_create_block_container(0, vaddr, 0);
  
  mjr_container_add_link(dst, tmpcntnr->id, MJR_LINK_FUNC_RET, MJR_LINK_IN);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
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
  int                   index, findex, tmptype, cnt;
  elfshsect_t           *sect, *flowsect;
  mjrcontainer_t	*curcntnr;
  mjrblock_t		*curblock;
  unsigned int		blocnbr, tmpid, off;
  char			name[20];
	
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_EDFMT_BLOCKS, 0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No control flow section : use analyse command", 0);
  if (sect->shdr->sh_size % sizeof(mjrblock_t))
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
  for (index = 0; index < blocnbr; index++)
    {
      
      // here you can see that the format ondisk is the exact same format that
      // we manipulate in this lib. So its very practical to reload/store the
      // info ondisk.
      curcntnr = (mjrcontainer_t *) sect->data + index;
      curblock = (mjrblock_t *) sect->data + index + sizeof(mjrcontainer_t);
      curcntnr->data = curblock;
      
      snprintf(name, sizeof(name), AFMT, curblock->vaddr);
      
#if __DEBUG_BLOCKS__
      fprintf(D_DESC,"[__DEBUG__] mjr_blocks_load: add new block name:%s\n",name);
#endif

      off = (u_int) curcntnr->input;
      curcntnr->input = NULL;
      
      /* fill the flow list */
      for (findex=0; findex < curcntnr->in_nbr; findex++)
	{	 
	  tmpid = *((unsigned int *) flowsect->data + off + findex);
	  tmptype = *((int *) flowsect->data + off + findex + sizeof(unsigned int));
	  
	  fprintf(D_DESC," [D] resotore parent: %u\n", tmpid);
	  mjr_container_add_link(curcntnr, tmpid, tmptype, MJR_LINK_IN);
	}

      off = (u_int)curcntnr->output;
      curcntnr->output = NULL;

      for (findex=0; findex < curcntnr->out_nbr; findex++)
	{
	  tmpid = *((unsigned int *)flowsect->data + off + findex);
	  tmptype = *((int *) flowsect->data + off + findex + sizeof(unsigned int));
	  
	  fprintf(D_DESC," [D] resotore child: %u\n", tmpid);
	  mjr_container_add_link(curcntnr, tmpid, tmptype, MJR_LINK_OUT);
	}

      mjr_register_container_id (curcntnr);
      hash_add(&ctxt->blkhash, (char *) _vaddr2str(curblock->vaddr), curcntnr);
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

  while (cur)
  {
    memcpy(buf->data + buf->maxlen, (char *)&cur->id , sizeof(unsigned int));
    buf->maxlen += sizeof(unsigned int);
    memcpy(buf->data + buf->maxlen, (char *)&cur->type , sizeof(int));
    buf->maxlen += sizeof(int);
    cur = cur->next;
  }

  curOff = buf->maxlen;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (curOff));
}
/**
 * Create the block information to be saved in file 
 */
int	mjr_block_save(mjrcontainer_t *cur, mjrbuf_t *buf)
{
  char			buffer[24];
  elfsh_Sym		bsym;
  elfsh_Sym		*sym;
  mjrblock_t		*curblock;
  mjrcontainer_t	*curcntnr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* At this points, no new block allocation should be done */
  curblock = cur->data;
  snprintf(buffer, sizeof (buffer), "block_%lX", (unsigned long) curblock->vaddr);
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

  else if (buf->allocated  < (buf->maxlen + sizeof(mjrblock_t)))
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

  /* Iteratively save all blocks */
  keys = hash_get_keys(&ctxt->blkhash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      u_int flow_off_in, flow_off_out;
      mjrlink_t *tmpin,*tmpout;

      tmpin=tmpout=NULL;

      blkcntnr = hash_get(&ctxt->blkhash, keys[index]);

      flow_off_in = mjr_block_flow_save(blkcntnr, 0, &cfbuf);
      flow_off_out = mjr_block_flow_save(blkcntnr, 1, &cfbuf);

      /* Setting references to link information inside the file */
      tmpin = blkcntnr->input;
      blkcntnr->input = (mjrlink_t *) flow_off_in;
      tmpout = blkcntnr->output;
      blkcntnr->output = (mjrlink_t *) flow_off_out;

      mjr_block_save(blkcntnr, &buf);
      block = (mjrblock_t *) blkcntnr->data;

      /* 
	 resotre old lists 
	 FIXME: we could retrieve the block once again
	 what do you think? /thorkill 200703
       */
      blkcntnr->input = tmpin;
      blkcntnr->output = tmpout;

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
  
  /* Exact match */
  if (mode == 0)
  {
    ret = hash_get(&ctxt->blkhash, _vaddr2str(vaddr));
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
  }

  /* Parent match */
  keys = hash_get_keys(&ctxt->blkhash, &size);
  for (index = 0; index < size; index++)
  {
    ret = hash_get(&ctxt->blkhash, keys[index]);
    tmpblock = (mjrblock_t *) ret->data;
    
    if (tmpblock->vaddr >= vaddr && vaddr <= tmpblock->vaddr + tmpblock->size)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
  }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}
