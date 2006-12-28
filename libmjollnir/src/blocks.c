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


/* Add a value in historic and shift all old ones */
void	mjr_history_shift(mjrcontext_t *cur, asm_instr vaddr) 
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  cur->vaddr_hist[0] = cur->vaddr_hist[1];
  cur->vaddr_hist[1] = cur->vaddr_hist[2];
  cur->vaddr_hist[2] = cur->vaddr_hist[3];
  cur->vaddr_hist[3] = cur->vaddr_hist[4];
  cur->vaddr_hist[4] = vaddr;  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, );
}


/* Say if 2 blocks are starting at the same address */
int			mjr_match_block(elfshiblock_t *elem, elfshiblock_t *match)
{
  int			cmp;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  cmp = match->vaddr - elem->vaddr;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, cmp);
}


/* Say if the the block 'match' starts inside block 'elem' */
int		mjr_match_inblock(elfshiblock_t *elem, elfshiblock_t *match)
{
  elfsh_Addr	cmp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  cmp = match->vaddr - elem->vaddr;
  if (cmp > 0 && cmp < elem->size)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, cmp);
}


/* Free linked list of elfshiblock_t (file representation of blocks) */
int	mjr_free_blocks(elfshiblock_t *blks)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (blks) 
    {      
      btree_free(blks->btree, 1);
      XFREE(blks);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Create the control flow graph using the information stored in .elfsh.control */
int			mjr_load_blocks(elfshobj_t *obj, elfshiblock_t **blist)
{
  elfsh_Sym             *symtab;
  int                   num;
  int                   index;
  int                   i;
  elfshsect_t           *sect;
  elfshiblock_t         *iblk;
  elfshblk_t            *blk;
  elfshblkref_t         *blkref;
  int                   load;
  void			*data;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  sect = elfsh_get_section_by_name(obj, ELFSH_SECTION_NAME_CONTROL, 0, 0, 0);
  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No control flow section saved", -1);
  data = elfsh_get_raw(sect);
  if (!data)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Control flow section has no data", -1);

  // If the function was already called, return the cached info
  if (sect->altdata)
    {
      *blist = sect->altdata;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* Iterate on symtab to find blocks */
  /* If block is already present present, skip it */
  symtab = elfsh_get_symtab(obj, &num);
  for (load = index = 0; index < num; index++)
    {
      if (elfsh_get_symbol_type(symtab + index) != STT_BLOCK)
        continue;
      blk          = data + symtab[index].st_value;
      if ((iblk = mjr_block_get_by_vaddr(*blist, blk->vaddr, 0)))
        continue;
      iblk         = mjr_block_create(blk->vaddr, blk->size);
      iblk->contig = blk->contig;
      iblk->altern = blk->altern;
      iblk->altype = blk->altype;       
      for (i = sizeof (elfshblk_t); i < symtab[index].st_size; 
           i += sizeof (elfshblkref_t))
        {
          blkref = data + (symtab + index)->st_value + i;
          mjr_block_add_caller(iblk, blkref->vaddr, blkref->type);
        }
      mjr_block_add_list(blist, iblk);
      load++;
    }
  sect->altdata = blist;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, load);
}

/* Say if a block is the start of a function or not */
int		 mjr_block_is_funcstart(elfshiblock_t *blk) 
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
int			mjr_save_block(elfshiblock_t *cur, elfshbuf_t *buf)
{
  u_int		        blen;
  int			is_func;
  char			buffer[24];
  elfshcaller_t		*cal;
  elfsh_Sym		bsym;
  elfsh_Sym		*sym;
  elfshblk_t		*curblock;
  elfshblkref_t		*blockref;
  int			index;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* 
  ** If that block was already allocated we should not allocate it
  ** again, since this function is called only on completely built 
  ** graphs and not graph beeing built, so there is no allocation size 
  ** change unless the control section was removed, and then its the first
  ** time the block is beeing allocated in the new section instance -may  
  */
  snprintf(buffer, sizeof (buffer), "block_%08x", cur->vaddr);
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
  else if (buf->allocated  < (buf->maxlen + sizeof (elfshblk_t)))
    {
      buf->allocated += getpagesize();
      buf->data = elfsh_realloc(buf->data, buf->allocated);
    }
  
  is_func = 0;
  blen = sizeof (elfshblk_t);
  
  curblock = (elfshblk_t *) (buf->data + buf->maxlen);
  curblock->vaddr  = cur->vaddr;
  curblock->contig = cur->contig;
  curblock->altern = cur->altern;
  curblock->size   = cur->size;
  curblock->altype = cur->altype;

  /* For each caller, add a new block reference to section */
  for (index = 0, cal = cur->caller; cal; cal = cal->next, index++) 
    {
      printf("index = %u and call = %08X \n", index, (unsigned int) cal);

      if (buf->allocated  < (buf->maxlen + blen + sizeof (elfshblkref_t)))
	{
	  buf->allocated += getpagesize();
	  buf->data = elfsh_realloc(buf->data, buf->allocated);
	}
      blockref = (elfshblkref_t *) (buf->data + buf->maxlen + blen);
      blockref->vaddr = cal->vaddr;
      blockref->type = cal->type;
      if (!is_func && cal->type == CALLER_CALL) 
	{
	  is_func = 1;
	  snprintf(buffer, sizeof (buffer), "function_%08x", cur->vaddr);
	} 
      blen += sizeof (elfshblkref_t);
      putchar('.');
    }

  /* Then we create the symbol for the bloc and returns */
  bsym = elfsh_create_symbol(buf->maxlen, blen, STT_BLOCK, 0, 0, 0);
  elfsh_insert_symbol(buf->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, buffer);
  buf->maxlen += blen;
  buf->block_counter++;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Store the blocks inside the .control section using the file representation */
int			mjr_store_blocks(elfshobj_t *obj , elfshiblock_t *blist, 
					 int mode) 
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  elfshbuf_t		buf;
  int			err;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  printf("Calling store blocks ! \n");

  /* For each block, insert a new STT_BLOCK symbol 
  ** and associated block in data of section */
  sect = elfsh_get_section_by_name(obj, ELFSH_SECTION_NAME_CONTROL, 0, 0, 0);
  if (sect)
    elfsh_remove_section(obj, ELFSH_SECTION_NAME_CONTROL);
  
  buf.allocated     = 0;
  buf.maxlen        = 0;
  buf.block_counter = 0;
  buf.data          = 0;
  buf.obj           = obj;

  if (mode)
    btree_browse_prefix(blist->btree, (void *) mjr_save_block, &buf);

  sect = elfsh_create_section(ELFSH_SECTION_NAME_CONTROL);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, buf.maxlen, 0, 0, 0, 0);
  sect->altdata = blist;

  printf(" [*] Saving control section of %u bytes \n", buf.maxlen);

  err = elfsh_insert_unmapped_section(obj, sect, shdr, buf.data);
  if (err < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to save control flow section", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.block_counter);
}



/* Reset the passed flag in all blocks of the list */
// NEVER CALLED !! to remove
/*
void		mjr_block_clean_passed(elfshiblock_t *list) 
{
  elfshiblock_t	*cur;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  for (cur = list; cur; cur = cur->next)
    cur->passed = 0;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, );
}
*/

/* Create a new block */
elfshiblock_t	*mjr_block_create(u_int vaddr, u_int size) 
{
  elfshiblock_t	*t;
  
  t = elfsh_malloc(sizeof (elfshiblock_t));
  memset(t, 0, sizeof (elfshiblock_t));
  t->vaddr = vaddr;
  t->size = size;
  return (t);
};


/* Print block */
void		mjr_block_dump(elfshiblock_t *b) 
{
  printf("[B]=(%08x) [V]=(%08x) sz=(%04u) \n", (int) b, b->vaddr, b->size);
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
  if (end_str == NULL || !(cur->contig))
    *buf2 = 0x00;
  else
    snprintf(buf2, sizeof (buf2), "<%s + " UFMT ">", end_str, end_offset);
      
  printf("[%08x:%05i:%08x:%08x] %-4s %-30s --> %-30s ", 
	 cur->vaddr, cur->size, cur->contig, cur->altern, 
	 call_type_str[cur->altype], buf1, buf2);
      
  if (cur->altern == 0xFFFFFFFF)
    printf(" [?]");
  else if (cur->altern != NULL)
    {
      str = elfsh_reverse_metasym(disopt->file, cur->altern, &offset);
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



/* Print the content of the control flow section */
int			mjr_display_blocks(elfshobj_t		*file, 
					   elfshiblock_t	*blk_list, 
					   int			level) 
{
  struct s_disopt	disopt;
  
  disopt.counter = 0;
  disopt.level = level;
  disopt.file = file;
  if (blk_list->btree)
    btree_browse_infix(blk_list->btree, (void *) mjr_apply_display, &disopt);
  return (disopt.counter);
}




/* Add a new block to the blocks tree (sorted by address)
** If block is already present, it's not inserted and function returns */
void		mjr_block_add_list(elfshiblock_t **list, elfshiblock_t *n) 
{
  elfshiblock_t	*cur;

  cur = *list;
  if (!cur)
    {
      cur = mjr_block_create(0, 0);
      *list = cur;
    }
  btree_insert_sort(&cur->btree, (void *) mjr_match_block, n);
}


  
/* Add a caller : vaddr is address of starting block */
void		mjr_block_add_caller(elfshiblock_t *blk, u_int vaddr, int type) 
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
elfshiblock_t	*mjr_block_get_by_vaddr(elfshiblock_t *list, u_int vaddr, int mode)
{
  elfshiblock_t	cur;
  elfshiblock_t	*bcur;
  
  if (!list)
    return (NULL);
  cur.vaddr = vaddr;
  bcur = btree_find_elem(list->btree, (mode ? 
				       (void *) mjr_match_inblock : 
				       (void *) mjr_match_block), &cur);
  return (bcur);
}




/* Make the link between the new block and the current block */
/* Split existing blocks if necessary */
int		mjr_block_point(elfshiblock_t **blklist,
				asm_instr      *ins,
				elfsh_Addr     vaddr,
				elfsh_Addr     dest)
{
  elfshiblock_t	*dst;
  elfshiblock_t	*dst_end;  
  int		new_size;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  
  dst_end = *blklist;
  dst     = mjr_block_get_by_vaddr(dst_end, dest, 1);

  /* Just create a new target block */
  if (!dst) 
    {
      dst = mjr_block_create(dest, 1);
      dst->altype = CALLER_UNKN;
      mjr_block_add_list(blklist, dst);
    } 

  /* Split block */
  else if (dst->vaddr != dest) 
    {
      new_size        = dst->size - (dest - dst->vaddr);
      dst->size      -= new_size;
      dst_end         = mjr_block_create(dest, new_size);
      dst_end->contig = dst->contig;
      dst_end->altern = dst->altern;
      dst_end->altype = dst->altype;
      dst->contig     = dest;
      dst->altype     = CALLER_CONT;
      dst->altern     = 0;
      mjr_block_add_list(blklist, dst_end);
      dst            = dst_end;
    }
  
  /* Add caller to target block */
  mjr_block_add_caller(dst, vaddr, 
		       (ins->instr == ASM_CALL ? CALLER_CALL : CALLER_JUMP));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* 
** Support function pointers computations
**
** Handle something like this : call *%reg
**     
**  -> Check goto table and fixup destination
**
**  - start gdb, set breakpoint on a known pointer function, run the prog
**  - check register value at breakpoint
**  - start elfsh
**  - addgoto <vaddr of call *%reg> <destaddr value of *%reg>   
*/
elfsh_Addr		mjr_compute_fctptr(mjrcontext_t		*context,
					   elfshobj_t		*obj, 
					   asm_instr		*ins, 
					   elfsh_Addr		vaddr,
					   elfshiblock_t	**blklist) 
{
  char		tmp[255];
  char		*ret;
  elfsh_Addr	dest;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  
  printf(" [*] Found function pointer at %x\n",vaddr);
  snprintf(tmp, sizeof(tmp), "%x", vaddr);

  /* We deal with a constructed address */
  if (context->vaddr_hist[4].instr == ASM_CALL &&
      context->vaddr_hist[3].instr == ASM_MOV  &&
      context->vaddr_hist[2].instr == ASM_MOV) 
    {
      dest = context->vaddr_hist[2].op2.imm;
      
      if (dest < elfsh_get_entrypoint(obj->hdr)) 
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid target vaddr for function pointer", 
			  (elfsh_Addr) -1);
      mjr_block_point(blklist, ins, vaddr, dest);

#if defined(__DEBUG_MJOLLNIR__)
      printf(" [*] 0x%08x Detected possible FUNCPTR at [%x/%d] \n",
	     vaddr, dest, dest);
#endif

      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
    } 
  
  /* Happens when an address was manually inserted in the routing table */
  /* This allow to avoid the control flow graph to be broken if elfsh
     is not capable to recompute the target address */
  ret = (char *) hash_get(&goto_hash, tmp);
  if (!ret) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to compute function pointer target", 
		      (elfsh_Addr) -1);  
  dest = strtol(ret, (char **) NULL, 16);
  if (dest) 
    {

#if defined(__DEBUG_MJOLLNIR__)
      printf(" [*] Extended routing table found 0x%08x -> 0x%08x\n", vaddr, dest);
#endif

      mjr_block_point(blklist, ins, vaddr, dest);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}





/*
 * This function add a new element to the linked list of callers 
 * of the current block.
 * 
 * It resolve operand of instruction which may modify the execution path.
 * If resolved to a virtual address, a new block is inserted.
 *
 * -> Return destination address inserted or -1 if unresolved
 */
int		mjr_insert_destaddr(mjrcontext_t	*context,
				    elfshobj_t		*obj, 
				    asm_instr		*ins, 
				    u_int		vaddr,
				    elfshiblock_t	**blklist) 
{
  int		ilen;
  elfshiblock_t	*dst_end;
  elfsh_Addr	dest;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  dest = 0;

  /* The target block is called directly */
  if ((ins->op1.content & ASM_OP_VALUE) && !(ins->op1.content & ASM_OP_REFERENCE)) 
    {    
      ilen = asm_instr_len(ins);
      asm_operand_get_immediate(ins, 1, 0, &dest);
      dest += ilen + vaddr;
      dst_end = *blklist;
      mjr_block_point(blklist, ins, vaddr, dest);
    }

  /* The target block is called indirectly : if we find a pattern that correspond 
     to an easy to predict function pointer, then we compute it */
  else if (ins->op1.content & ASM_OP_BASE) 
    dest = mjr_compute_fctptr(context, obj, ins, vaddr, blklist);
  else
    dest = -1;
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}





/*---------- Original libmjollnir block format !!!!!! --------- */


/* Create a block in the original MJR format */
mjrblock_t	*mjr_create_block(u_int vaddr, char *section, u_int type) 
{
  mjrblock_t	*nblk;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(nblk, sizeof(mjrblock_t), NULL);
  nblk->vaddr    = vaddr;
  nblk->type     = type;
  nblk->name     = NULL;
  nblk->section  = elfsh_strdup(section);
  nblk->flowTo   = NULL;
  nblk->flowRet  = NULL;
  nblk->listNext = NULL;
  nblk->listPrev = NULL;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (nblk));
}


