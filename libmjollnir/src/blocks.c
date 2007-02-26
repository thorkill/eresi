/*
** blocks.c in libmjollnir for elfsh
** 
** Authors : sk, mayhem, thorkill
**
** Started : Thu May 29 20:39:14 2003 sk
** Updated : Fri Dec 15 01:09:47 2006 mayhem
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
int			mjr_block_point(mjrcontext_t  *ctxt, 
					asm_instr      *ins,
					elfsh_Addr     vaddr,
					elfsh_Addr     dest)
{
  mjrblock_t	*dst;
  mjrblock_t	*dst_end;  
  int		new_size;
  int		type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  dst_end = ctxt->blklist;
  dst     = mjr_block_get_by_vaddr(ctxt, dest, 1);

  /* Just create a new target block */
  if (!dst) 
    {
      dst = mjr_block_create(ctxt, dest, 1);
      dst->type = CALLER_UNKN;
      mjr_block_add_list(ctxt, dst);
    } 

  /* Split block */
  else if (dst->vaddr != dest) 
    {
      new_size        = dst->size - (dest - dst->vaddr);
      dst->size      -= new_size;
      dst_end         = mjr_block_create(ctxt, dest, new_size);
      dst_end->true   = dst->true;
      dst_end->false  = dst->false;
      dst_end->type   = dst->type;
      dst->true       = dest;
      dst->type       = CALLER_CONT;
      dst->false      = 0;
      
      mjr_block_add_list(ctxt, dst_end);
      dst             = dst_end;
    }
  
  /* Add caller to target block */
  if (ctxt->proc.type == ASM_PROC_IA32)
    {
      type = ins->instr == ASM_CALL ? CALLER_CALL : CALLER_JUMP;
    }
  else if (ctxt->proc.type == ASM_PROC_SPARC)
    {
      type = ins->instr == ASM_SP_CALL ? CALLER_CALL : CALLER_JUMP;
    }
  mjr_block_add_caller(dst, vaddr, type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Retreive control flow section content if any 
 */
mjrblock_t*	mjr_blocks_get(mjrcontext_t *ctxt)
{
  elfshsect_t	*sect;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
 /* Parse arguments, load binary and resolve symbol */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_CONTROL, 0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No control flow section : use analyse command", NULL);

  /* Return or retreive information */
  if (sect->altdata)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);

  sect->altdata = mjr_blocks_load(ctxt);

  if (sect->altdata)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Flow analysis failed", 0);
}


/**
 * Create the control flow graph using the information stored in .elfsh.control 
 */
mjrblock_t*		mjr_blocks_load(mjrcontext_t *ctxt)
{
  int                   index;
  elfshsect_t           *sect;
  mjrblock_t            *curbloc;
  mjrblock_t            *target;
  unsigned int		blocnbr;
  char			name[20];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_CONTROL, 0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No control flow section : use analyse command", 0);
  if (sect->shdr->sh_size % sizeof(mjrblock_t))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Corrupted control flow section : modulo-test failed", 0);

  /* If the function was already called, return its result */
  if (sect->altdata)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);
  
  /* First pass : Iterate on the control flow section to find blocks */
  /* We do not create new blocks but use the data from the section */
  blocnbr = sect->shdr->sh_size / sizeof(mjrblock_t);
  for (index = 0; index < blocnbr; index++)
    {

      // here you can see that the format ondisk is the exact same format that
      // we manipulate in this lib. So its very practical to reload/store the
      // info ondisk.
      curbloc = (mjrblock_t *) sect->data + index;
      mjr_block_add_list(ctxt, curbloc);
      snprintf(name, sizeof(name), AFMT, curbloc->vaddr);

#if __DEBUG_BLOCKS__
      fprintf(D_DESC,"[__DEBUG__] mjr_blocks_load: add new block name:%s/%d\n",name,
        curbloc->type);
#endif

      hash_add(&ctxt->blkhash, (char *) _vaddr2str(curbloc->vaddr), curbloc);
    }
  
  /* Second pass : create all caller information for all loaded blocks */
  for (index = 0; index < blocnbr; index++)
    {
      curbloc = (mjrblock_t *) sect->data + index;
 
      /* Link true child info */
      target = mjr_block_get_by_vaddr(ctxt, curbloc->true, 0);

#if __DEBUG_BLOCKS__
      if (!target)
	fprintf(D_DESC,"[__DEBUG__] mjr_blocks_load: 1-pass vaddr: "XFMT"\n",
		curbloc->true);
#endif

      if (!target)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Corrupted control flow callers information 1", 0);

      mjr_block_add_caller(target, curbloc->vaddr, curbloc->type);

      /* Link false child info */
      target = mjr_block_get_by_vaddr(ctxt, curbloc->false, 0);

#if __DEBUG_BLOCKS__
      if (!target)
	fprintf(D_DESC,"[__DEBUG__] mjr_blocks_load: 2-pass vaddr: "XFMT"\n",
		curbloc->false);
#endif

      if (!target)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Corrupted control flow callers information 2", 0);

      mjr_block_add_caller(target, curbloc->vaddr, curbloc->type);
    }

  /* Return results */
  sect->altdata = ctxt->blklist;

  /* FIXME: prevent double analysis */
  ctxt->analysed = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);
}


/**
 * Create the block information to be saved in file 
 */
int			mjr_block_save(mjrblock_t *cur, mjrbuf_t *buf)
{
  char			buffer[24];
  elfsh_Sym		bsym;
  elfsh_Sym		*sym;
  mjrblock_t		*curblock;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* At this points, no new block allocation should be done */
  snprintf(buffer, sizeof (buffer), "block_%lX", (unsigned long) cur->vaddr);
  sym = elfsh_get_symbol_by_name(buf->obj, buffer);
  if (sym)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);

#if __DEBUG_BLOCKS__
  fprintf(D_DESC," [*] Saving block at addr %s \n", buffer);
#endif

  /* Else insert the block in the global buffer for the .control section */
  if (!buf->data) 
    {
      buf->allocated = getpagesize();;
      XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     buf->data, buf->allocated, -1);
      buf->maxlen = 0;
    }
  else if (buf->allocated  < (buf->maxlen + sizeof(mjrblock_t)))
    {
      buf->allocated += getpagesize();
      XREALLOC(__FILE__, __FUNCTION__, __LINE__,
	       buf->data, buf->data, buf->allocated, -1);
    }
  curblock         = (mjrblock_t *) ((char *) buf->data + buf->maxlen);
  memcpy(curblock, cur, sizeof(mjrblock_t));
  curblock->caller = NULL;

  /* Then we create the symbol for the bloc and returns */
  bsym = elfsh_create_symbol(cur->vaddr, cur->size, STT_BLOCK, 0, 0, 0);
  elfsh_insert_symbol(buf->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, buffer);
  buf->maxlen += sizeof(mjrblock_t);
  buf->block_counter++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Store the blocks inside the .control section using the file representation 
 */
// This loops on all block, and call blocks_save on each bloc
int			mjr_blocks_store(mjrcontext_t *ctxt) 
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  mjrbuf_t		buf;
  mjrblock_t		*block;
  mjrfunc_t		*func;
  int			err;
  char			**keys;
  int			keynbr;
  int			index;
  char			funcname[50];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Remove previous control section if any */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_CONTROL, 0, 0, 0);
  if (sect)
    elfsh_remove_section(ctxt->obj, ELFSH_SECTION_NAME_CONTROL);

  /* Initialize data buffer */
  buf.allocated     = 0;
  buf.maxlen        = 0;
  buf.block_counter = 0;
  buf.data          = 0;
  buf.obj           = ctxt->obj;

  /* Iteratively save all blocks */
  keys = hash_get_keys(&ctxt->blkhash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      block = hash_get(&ctxt->blkhash, keys[index]);
      mjr_block_save(block, &buf);

      if (mjr_block_funcstart(block))
	{
	  snprintf(funcname, sizeof(funcname), AFMT, block->vaddr);
	  func        = hash_get(&ctxt->funchash, funcname);

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
  sect = elfsh_create_section(ELFSH_SECTION_NAME_CONTROL);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, buf.maxlen, 0, 0, 0, 0);
  sect->altdata = ctxt->blklist = (mjrblock_t *) buf.data;

  printf(" [*] Saving control section of %u bytes \n", buf.maxlen);
  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, buf.data);

  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to save control flow section", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.block_counter);
}


/**
 * Create a new block 
 */
mjrblock_t	*mjr_block_create(mjrcontext_t *ctxt, elfsh_Addr vaddr, u_int sz) 
{
  mjrblock_t	*t;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, t, sizeof (mjrblock_t), NULL);
  memset(t, 0, sizeof (mjrblock_t));
  t->vaddr = vaddr;
  t->size  = sz;
  hash_add(&ctxt->blkhash, (char *) _vaddr2str(vaddr), t);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (t));
};



/**
 * Add a new block to the blocks tree (sorted by address)
 * If block is already present, it's not inserted and function returns 
 */
void		mjr_block_add_list(mjrcontext_t *ctxt, mjrblock_t *n) 
{
  mjrblock_t	*cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  cur = ctxt->blklist;
  if (!cur)
    {
      cur = mjr_block_create(ctxt, n->vaddr, n->size);
      ctxt->blklist = cur;
    }
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/** 
 * Add a caller : vaddr is address of starting block 
 */
void		mjr_block_add_caller(mjrblock_t *blk, 
				     elfsh_Addr    vaddr, 
				     int	   type) 
{
  mjrlink_t	*n;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, n, sizeof (mjrlink_t), );
  n->vaddr = vaddr;
  n->type = type;
  n->next = blk->caller;
  blk->caller = n;

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/** 
 * Get a block by vaddr
 *
 * If mode = 0, return block only if vaddr is equal to block starting address
 * else return block if vaddr belong to block
 */
mjrblock_t	*mjr_block_get_by_vaddr(mjrcontext_t *ctxt, 
					elfsh_Addr   vaddr, 
					int	     mode)
{
  mjrblock_t	*ret;
  char		**keys;
  int		index;
  int		size;

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
      ret = (mjrblock_t *) hash_get(&ctxt->blkhash, keys[index]);
      if (ret->vaddr >= vaddr && vaddr <= ret->vaddr + ret->size)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(NULL));
}
