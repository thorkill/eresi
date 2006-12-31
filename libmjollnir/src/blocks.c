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
  elfshiblock_t	*dst;
  elfshiblock_t	*dst_end;  
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
elfshiblock_t*	mjr_blocks_get(mjrcontext_t *ctxt)
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
elfshiblock_t*		mjr_blocks_load(mjrcontext_t *ctxt)
{
  int                   index;
  elfshsect_t           *sect;
  elfshiblock_t         *curbloc;
  elfshiblock_t         *target;
  unsigned int		blocnbr;
  char			name[20];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_CONTROL, 0, 0, 0);
  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No control flow section : use analyse command", 0);
  if (sect->shdr->sh_size % sizeof(elfshiblock_t))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Corrupted control flow section : modulo-test failed", 0);

  /* If the function was already called, return its result */
  if (sect->altdata)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);
  
  /* First pass : Iterate on the control flow section to find blocks */
  /* We do not create new blocks but use the data from the section */
  blocnbr = sect->shdr->sh_size / sizeof(elfshiblock_t);
  for (index = 0; index < blocnbr; index++)
    {
      curbloc = (elfshiblock_t *) sect->data + index;
      mjr_block_add_list(ctxt, curbloc);
      snprintf(name, sizeof(name), AFMT, curbloc->vaddr);
      hash_add(&ctxt->blkhash, name, curbloc);
    }
     
  /* Second pass : create all caller information for all loaded blocks */
  for (index = 0; index < blocnbr; index++)
    {
      curbloc = (elfshiblock_t *) sect->data + index;
 
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
int		 mjr_block_funcstart(elfshiblock_t *blk) 
{
  elfshcaller_t	 *cur;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (blk)
    for (cur = blk->caller; cur; cur = cur->next)
      if (cur->type == CALLER_CALL)
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Create the block information to be saved in file */
int			mjr_block_save(elfshiblock_t *cur, elfshbuf_t *buf)
{
  char			buffer[24];
  elfsh_Sym		bsym;
  elfsh_Sym		*sym;
  elfshiblock_t		*curblock;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* At this points, no new block allocation should be done */
  snprintf(buffer, sizeof (buffer), "block_%lX", (unsigned long) cur->vaddr);
  sym = elfsh_get_symbol_by_name(buf->obj, buffer);
  if (sym)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);

  printf("Saving block at addr %s \n", buffer);

  /* Else insert the block in the global buffer for the .control section */
  if (!buf->data) 
    {
      buf->allocated = getpagesize();;
      buf->data = elfsh_malloc(buf->allocated);
      buf->maxlen = 0;
    }
  else if (buf->allocated  < (buf->maxlen + sizeof(elfshiblock_t)))
    {
      buf->allocated += getpagesize();
      buf->data = elfsh_realloc(buf->data, buf->allocated);
    }
  
  curblock         = (elfshiblock_t *) ((char *) buf->data + buf->maxlen);
  memcpy(curblock, cur, sizeof(elfshiblock_t));
  curblock->caller = NULL;

  /* Then we create the symbol for the bloc and returns */
  bsym = elfsh_create_symbol(cur->vaddr, cur->size, STT_BLOCK, 0, 0, 0);
  elfsh_insert_symbol(buf->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, buffer);
  buf->maxlen += sizeof(elfshiblock_t);
  buf->block_counter++;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Recursive traversal of the graph */
static int		mjr_block_recstore(mjrcontext_t *ctxt,
					   elfshiblock_t *start, 
					   elfshbuf_t *buf)
{
  elfshiblock_t		*tmp;

  if (mjr_block_save(start, buf) < 0)
    return (-1);
  tmp = mjr_block_get_by_vaddr(ctxt, start->true, 0);
  assert(tmp != 0);
  if (mjr_block_recstore(ctxt, tmp, buf) < 0)
    return (-1);
  if (start->false)
    {
      tmp = mjr_block_get_by_vaddr(ctxt, start->false, 0);
      assert(tmp != 0);
      if (mjr_block_recstore(ctxt, tmp, buf) < 0)
	return (-1);
    }
  return (0);
}


/* Store the blocks inside the .control section using the file representation */
int			mjr_blocks_store(mjrcontext_t *ctxt) 
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  elfshbuf_t		buf;
  elfshiblock_t		*start;
  int			err;
  elfsh_Addr		e_entry;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  printf("Calling store blocks ! \n");

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

  /* Recursively save blocks information and create section */
  e_entry = elfsh_get_entrypoint(ctxt->obj->hdr);
  start = mjr_block_get_by_vaddr(ctxt, e_entry, 0);
  mjr_block_recstore(ctxt, start, &buf);
  sect = elfsh_create_section(ELFSH_SECTION_NAME_CONTROL);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, buf.maxlen, 0, 0, 0, 0);
  sect->altdata = ctxt->blklist;

  printf(" [*] Saving control section of %u bytes \n", buf.maxlen);
  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, buf.data);
  if (err < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to save control flow section", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.block_counter);
}


/* Create a new block */
elfshiblock_t	*mjr_block_create(mjrcontext_t *ctxt, elfsh_Addr vaddr, u_int size) 
{
  elfshiblock_t	*t;
  char		tmp[20];

  t = elfsh_malloc(sizeof (elfshiblock_t));
  memset(t, 0, sizeof (elfshiblock_t));
  t->vaddr = vaddr;
  t->size = size;
  snprintf(tmp, sizeof(tmp), AFMT, vaddr);
  hash_add(&ctxt->blkhash, tmp, t);
  return (t);
};


/* Print block */
void		mjr_block_dump(elfshiblock_t *b) 
{
  printf("[B]=(%lX) [V]=(%lX) sz=(%04u) \n", 
	 (unsigned long) b, (unsigned long) b->vaddr, b->size);
}


/* Display all information about a block */
int			mjr_apply_display(elfshiblock_t *cur, void *opt)
{
  elfshcaller_t		*ccal;
  struct s_disopt	*disopt;
  char			*str;
  char			*end_str;
  elfsh_SAddr		offset;
  elfsh_SAddr		end_offset;
  char			buf1[30];
  char			buf2[30];
  
  disopt = (struct s_disopt *) opt;
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
  return (++disopt->counter);
}


/* Recursive traversal of the graph */
static int		mjr_block_recdisplay(mjrcontext_t  *c,
					     elfshiblock_t *start, 
					     elfshopt_t    *opt)
{
  elfshiblock_t		*tmp;

  if (mjr_apply_display(start, opt) < 0)
    return (-1);
  tmp = mjr_block_get_by_vaddr(c, start->true, 0);
  assert(tmp != 0);
  if (mjr_block_recdisplay(c, tmp, opt) < 0)
    return (-1);
  if (start->false)
    {
      tmp = mjr_block_get_by_vaddr(c, start->false, 0);
      assert(tmp != 0);
      if (mjr_block_recdisplay(c, tmp, opt) < 0)
	return (-1);
    }
  return (0);
}


/* Print the content of the control flow section */
int		mjr_blocks_display(mjrcontext_t	*c, int level)
{
  elfshopt_t	opt;
  elfsh_Addr	e_entry;
  elfshiblock_t	*start;

  e_entry     = elfsh_get_entrypoint(c->obj->hdr);
  start       = mjr_block_get_by_vaddr(c, e_entry, 0);
  opt.counter = 0;
  opt.level   = level;
  opt.file    = c->obj;
  mjr_block_recdisplay(c, start, &opt);
  return (opt.counter);
}




/* Add a new block to the blocks tree (sorted by address)
** If block is already present, it's not inserted and function returns */
void		mjr_block_add_list(mjrcontext_t *ctxt, elfshiblock_t *n) 
{
  elfshiblock_t	*cur;

  cur = ctxt->blklist;
  if (!cur)
    {
      cur = mjr_block_create(ctxt, cur->vaddr, cur->size);
      ctxt->blklist = cur;
    }
}


  
/* Add a caller : vaddr is address of starting block */
void		mjr_block_add_caller(elfshiblock_t *blk, 
				     elfsh_Addr    vaddr, 
				     int	   type) 
{
  elfshcaller_t	*n;
  
  n = elfsh_malloc(sizeof (elfshcaller_t));
  n->vaddr = vaddr;
  n->type = type;
  n->next = blk->caller;
  blk->caller = n;
}


/* 
 * Get a block by vaddr
 *
 * If mode = 0, return block only if vaddr is equal to block starting address
 * else return block if vaddr belong to block
 */
elfshiblock_t	*mjr_block_get_by_vaddr(mjrcontext_t *ctxt, 
					elfsh_Addr   vaddr, 
					int	     mode)
{
  elfshiblock_t	cur;
  elfshiblock_t	*ret;
  char		**keys;
  int		index;
  int		size;

  if (!ctxt)
    return (NULL);
  cur.vaddr = vaddr;
  keys = hash_get_keys(&ctxt->blkhash, &size);
  for (index = 0; index < size; index++)
    {
      ret = (elfshiblock_t *) hash_get(&ctxt->blkhash, keys[index]);
      switch (mode)
	{
	  /* Return exact match */
	case 0:
	  if (ret->vaddr == vaddr)
	    return (ret);
	  break;

	  /* Return parent match */
	default:
	  if (ret->vaddr >= vaddr && vaddr <= ret->vaddr + ret->size)
	    return (ret);
	  break;
	}
    }
  return (NULL);
}
