/*
** blocks.c in libmjollnir for elfsh
** 
** Authors : sk, mayhem, thorkill
**
** Started : Thu May 29 20:39:14 2003 sk
** Updated : Fri Dec 15 01:09:47 2006 mayhem
*/
#include "libmjollnir.h"

/* Some constant data */
char	*call_type_str[] = 
{
  "CONT",
  "JUMP",
  "CALL",
  "RET",
  "UNKN"
};

/* Goto hash */
hash_t		goto_hash;


/* Make the link between the new block and the current block */
/* Split existing blocks if necessary */
int		mjr_block_point(mjrcontext_t  *ctxt, 
				asm_instr      *ins,
				elfsh_Addr     vaddr,
				elfsh_Addr     dest)
{
  mjrblock_t	*dst;
  mjrblock_t	*dst_end;  
  int		new_size;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  
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
  mjr_block_add_caller(dst, vaddr, 
		       (ins->instr == ASM_CALL ? CALLER_CALL : CALLER_JUMP));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Retreive control flow section content if any */
mjrblock_t*	mjr_blocks_get(mjrcontext_t *ctxt)
{
  elfshsect_t	*sect;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
 /* Parse arguments, load binary and resolve symbol */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_CONTROL, 0, 0, 0);
  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No control flow section : use analyse command", NULL);

  /* Return or retreive information */
  if (sect->altdata)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);
  sect->altdata = mjr_blocks_load(ctxt);
  if (sect->altdata)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Flow analysis failed", 0);
}


/* Create the control flow graph using the information stored in .elfsh.control */
mjrblock_t*		mjr_blocks_load(mjrcontext_t *ctxt)
{
  int                   index;
  elfshsect_t           *sect;
  mjrblock_t         *curbloc;
  mjrblock_t         *target;
  unsigned int		blocnbr;
  char			name[20];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_CONTROL, 0, 0, 0);
  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No control flow section : use analyse command", 0);
  if (sect->shdr->sh_size % sizeof(mjrblock_t))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Corrupted control flow section : modulo-test failed", 0);

  /* If the function was already called, return its result */
  if (sect->altdata)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);
  
  /* First pass : Iterate on the control flow section to find blocks */
  /* We do not create new blocks but use the data from the section */
  blocnbr = sect->shdr->sh_size / sizeof(mjrblock_t);
  for (index = 0; index < blocnbr; index++)
    {
      curbloc = (mjrblock_t *) sect->data + index;

      mjr_block_add_list(ctxt, curbloc);
      snprintf(name, sizeof(name), AFMT, curbloc->vaddr);

      fprintf(D_DESC,"[__DEBUG__] mjr_blocks_load: add new block name:%s\n",name);

      hash_add(&ctxt->blkhash, name, curbloc);
    }
     
  /* Second pass : create all caller information for all loaded blocks */
  for (index = 0; index < blocnbr; index++)
    {
      curbloc = (mjrblock_t *) sect->data + index;
 
      /* Link true child info */
      target = mjr_block_get_by_vaddr(ctxt, curbloc->true, 0);
      if (!target)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Corrupted control flow callers information 1", 0);
      mjr_block_add_caller(target, curbloc->vaddr, curbloc->type);

      /* Link false child info */
      target = mjr_block_get_by_vaddr(ctxt, curbloc->false, 0);
      if (!target)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Corrupted control flow callers information 2", 0);
      mjr_block_add_caller(target, curbloc->vaddr, curbloc->type);
    }

  /* Return results */
  sect->altdata = ctxt->blklist;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);
}



/* Say if a block is the start of a function or not */
int		 mjr_block_funcstart(mjrblock_t *blk) 
{
  mjrcaller_t	 *cur;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (blk)
    for (cur = blk->caller; cur; cur = cur->next)
      if (cur->type == CALLER_CALL)
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Create the block information to be saved in file */
int			mjr_block_save(mjrblock_t *cur, mjrbuf_t *buf)
{
  char			buffer[24];
  elfsh_Sym		bsym;
  elfsh_Sym		*sym;
  mjrblock_t		*curblock;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* At this points, no new block allocation should be done */
  snprintf(buffer, sizeof (buffer), "block_%lX", (unsigned long) cur->vaddr);
  sym = elfsh_get_symbol_by_name(buf->obj, buffer);
  if (sym)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);

  printf(" [*] Saving block at addr %s \n", buffer);

  /* Else insert the block in the global buffer for the .control section */
  if (!buf->data) 
    {
      buf->allocated = getpagesize();;
      buf->data = elfsh_malloc(buf->allocated);
      buf->maxlen = 0;
    }
  else if (buf->allocated  < (buf->maxlen + sizeof(mjrblock_t)))
    {
      buf->allocated += getpagesize();
      buf->data = elfsh_realloc(buf->data, buf->allocated);
    }
  curblock         = (mjrblock_t *) ((char *) buf->data + buf->maxlen);
  memcpy(curblock, cur, sizeof(mjrblock_t));
  curblock->caller = NULL;

  /* Then we create the symbol for the bloc and returns */
  bsym = elfsh_create_symbol(cur->vaddr, cur->size, STT_BLOCK, 0, 0, 0);
  elfsh_insert_symbol(buf->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, buffer);
  buf->maxlen += sizeof(mjrblock_t);
  buf->block_counter++;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Store the blocks inside the .control section using the file representation */
int			mjr_blocks_store(mjrcontext_t *ctxt) 
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  mjrbuf_t		buf;
  mjrblock_t		*block;
  int			err;
  char			**keys;
  int			keynbr;
  int			index;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
    }

  /* Create control section */
  sect = elfsh_create_section(ELFSH_SECTION_NAME_CONTROL);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, buf.maxlen, 0, 0, 0, 0);
  sect->altdata = ctxt->blklist = (mjrblock_t *) buf.data;

  printf(" [*] Saving control section of %u bytes \n", buf.maxlen);
  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, buf.data);
  if (err < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to save control flow section", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.block_counter);
}


/* Create a new block */
mjrblock_t	*mjr_block_create(mjrcontext_t *ctxt, elfsh_Addr vaddr, u_int sz) 
{
  mjrblock_t	*t;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  t = elfsh_malloc(sizeof (mjrblock_t));
  memset(t, 0, sizeof (mjrblock_t));
  t->vaddr = vaddr;
  t->size  = sz;
  hash_add(&ctxt->blkhash, (char *) _vaddr2str(vaddr), t);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (t));
};


/* Print block */
void		mjr_block_dump(mjrblock_t *b) 
{
  printf("[B]=(%lX) [V]=(%lX) sz=(%04u) \n", 
	 (unsigned long) b, (unsigned long) b->vaddr, b->size);
}


/* Display all information about a block */
int			mjr_block_display(mjrblock_t *cur, mjropt_t *disopt)
{
  mjrcaller_t		*ccal;
  char			*str;
  char			*end_str;
  elfsh_SAddr		offset;
  elfsh_SAddr		end_offset;
  char			buf1[30];
  char			buf2[30];
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  str = elfsh_reverse_metasym(disopt->file, cur->vaddr, &offset);
  end_str = elfsh_reverse_metasym(disopt->file, 
				  cur->vaddr + cur->size, &end_offset);

  if (str == NULL)
    *buf1 = 0x00;
  else
    snprintf(buf1, sizeof (buf1), "<%s + " UFMT ">", str, offset);
  if (end_str == NULL || !(cur->true))
    *buf2 = 0x00;
  else
    snprintf(buf2, sizeof (buf2), "<%s + " UFMT ">", end_str, end_offset);
      
  printf("[%8lx:%05i:%8lx:%8lx] %-4s %-30s --> %-30s ", 
	 (unsigned long) cur->vaddr, cur->size, (unsigned long) cur->true, 
	 (unsigned long) cur->false, call_type_str[cur->type], buf1, buf2);
      
  if (cur->false == 0xFFFFFFFF)
    printf(" [?]");
  else if (cur->false != NULL)
    {
      str = elfsh_reverse_metasym(disopt->file, cur->false, &offset);
      printf(" [%s + " UFMT "]", (str ? str : ""), offset);
    }
      
  printf("\n");
  if (disopt->level > 0)
    for (ccal = cur->caller; ccal; ccal = ccal->next) 
      {
	str = elfsh_reverse_metasym(disopt->file, ccal->vaddr, &offset);
	printf("\texecuted from: (%08x) <%s + " UFMT "> : %s\n",
	       ccal->vaddr, (str ? str : ""), (elfsh_SAddr) offset, 
	       call_type_str[ccal->type]);
      }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ++disopt->counter);
}


/* Print the content of the control flow section */
int		mjr_blocks_display(mjrcontext_t	*c, int level)
{
  mjropt_t	opt;
  mjrblock_t	*block;
  char		**keys;
  int		index;
  int		blocnbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  opt.counter = 0;
  opt.level   = level;
  opt.file    = c->obj;
  keys        = hash_get_keys(&c->blkhash, &blocnbr);
  for (index = 0; index < blocnbr; index++)
    {
      block = hash_get(&c->blkhash, keys[index]);
      mjr_block_display(block, &opt);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, opt.counter);
}




/* Add a new block to the blocks tree (sorted by address)
** If block is already present, it's not inserted and function returns */
void		mjr_block_add_list(mjrcontext_t *ctxt, mjrblock_t *n) 
{
  mjrblock_t	*cur;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  cur = ctxt->blklist;
  if (!cur)
    {
      cur = mjr_block_create(ctxt, n->vaddr, n->size);
      ctxt->blklist = cur;
    }
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


  
/* Add a caller : vaddr is address of starting block */
void		mjr_block_add_caller(mjrblock_t *blk, 
				     elfsh_Addr    vaddr, 
				     int	   type) 
{
  mjrcaller_t	*n;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  n = elfsh_malloc(sizeof (mjrcaller_t));
  n->vaddr = vaddr;
  n->type = type;
  n->next = blk->caller;
  blk->caller = n;

  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* 
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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!ctxt)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "missing context", (NULL));

  /* Exact match */
  if (mode == 0)
    {
      ret = hash_get(&ctxt->blkhash, _vaddr2str(vaddr));
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
    }

  /* Parent match */
  keys = hash_get_keys(&ctxt->blkhash, &size);
  for (index = 0; index < size; index++)
    {
      ret = (mjrblock_t *) hash_get(&ctxt->blkhash, keys[index]);
      if (ret->vaddr >= vaddr && vaddr <= ret->vaddr + ret->size)
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,(NULL));
}


/* Shortcut for vaddr 2 string with allocation .. */
char	*_vaddr2str(elfsh_Addr addr)
{
  char *tmp;
  
  tmp = elfsh_malloc(BSIZE_SMALL);
  snprintf(tmp, BSIZE_SMALL - 1, AFMT, addr);
  return ((char *) tmp);
}
