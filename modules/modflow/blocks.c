/*
** 
** sk_blocks.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Thu May 29 20:39:14 2003
** Updated : Fri Nov 28 01:09:47 2003
*/

#include "modflow.h"

extern hash_t *s_goto;
extern struct s_elfshstng mf_settings;

char	*call_type_str[] = 
{
  "CONT",
  "JUMP",
  "CALL",
  "RET",
  "UNKN"
};

/**
 *
 * btree stuff
 *
 */


int	match_block(void *elem, void *match)
{
  struct s_iblock	*blk_elem;
  struct s_iblock	*blk_match;
  int			cmp;
  
  blk_match = (struct s_iblock *) match;
  blk_elem = (struct s_iblock *) elem;
  /*
  printf("cur=(%08x) max=(%08x) match=(%08x)\n", 
	 blk_elem->vaddr, blk_elem->vaddr + blk_elem->size, blk_match->vaddr);
  */
  cmp = blk_match->vaddr - blk_elem->vaddr;
  return (cmp);
}

int	match_inblock(void *elem, void *match)
{
  struct s_iblock	*blk_elem;
  struct s_iblock	*blk_match;
  int			cmp;
  blk_match = (struct s_iblock *) match;
  blk_elem = (struct s_iblock *) elem;
  cmp = blk_match->vaddr - blk_elem->vaddr;
  /*
  printf("cur=(%08x) max=(%08x) match=(%08x)", 
	 blk_elem->vaddr, blk_elem->vaddr + blk_elem->size, blk_match->vaddr);
  */
  if (cmp > 0)
    if (cmp < blk_elem->size)
      {
	// puts(" match !");
	return (0);
      }
  // puts("");
  return (cmp);
}

/*
 *
 * free linked list of struct s_iblock *
 *
 */

void	free_blocks(struct s_iblock *blks)
{
  if (blks) 
    {      
      btree_free(blks->btree, 1);
      XFREE(blks);
    }
}

/**
 *
 * load linked list
 *
 */

int     load_blocks(elfshobj_t *obj, struct s_iblock **blist)
{
  elfsh_Sym             *symtab;
  int                   num;
  int                   index;
  int                   i;
  elfshsect_t           *sect;
  struct s_iblock       *iblk;
  elfshblk_t            *blk;
  elfshblkref_t         *blkref;
  int                   load;
  void			*data;

  //*blist = 0;
  if (!(sect = elfsh_get_section_by_name(world.curjob->current, ".control", 0, 0, 0)))
    {
      puts(" * no \".control\" section\n");
      return (-1);
    }

  data = elfsh_get_raw(sect);
  if (!data)
    {
      puts(" * section \".control\" has no data\n");
      return (-1);
    }
  
  if (sect->altdata)
    {
      *blist = sect->altdata;
      return (42);
    }
  
  symtab = elfsh_get_symtab(world.curjob->current, &num);
  
  for (load = index = 0; index < num; index++)
    {
      if ((elfsh_get_symbol_type(symtab + index) != STT_BLOCK))
        continue;
      blk = (data + (symtab + index)->st_value);
      
      /* if block is already present present, skip it   */
      if ((iblk = block_get_by_vaddr(*blist, blk->vaddr, 0)))
        continue;
      iblk = block_create(blk->vaddr, blk->size);
      iblk->contig = blk->contig;
      iblk->altern = blk->altern;
      iblk->altype = blk->altype;       
      for (i = sizeof (elfshblk_t); i < (symtab + index)->st_size; 
           i += sizeof (elfshblkref_t))
        {
          blkref = data + (symtab + index)->st_value + i;
          block_add_caller(iblk, blkref->vaddr, blkref->type);
          
        }
      block_add_list(blist, iblk);
      load++;
    }
  sect->altdata = blist;
  return (load);
}


int	block_is_called(struct s_iblock *blk) 
{
  struct s_caller	*cur;
  
  if (blk)
    for (cur = blk->caller; cur; cur = cur->next) {
      if (cur->type == CALLER_CALL)
	return (1);
  }
  return (0);
}

/*
void	load_blocks()
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  
}
*/

int			build_data(void *elem, void *opt)
{
  u_int		        blen;
  int			is_func;
  char			buffer[24];
  struct s_buf		*buf;
  struct s_iblock	*cur;
  struct s_caller	*cal;
  elfsh_Sym		bsym;
  elfshblk_t		*curblock;
  elfshblkref_t		*blockref;
  
  buf = (struct s_buf *) opt;
  cur = (struct s_iblock *) elem;
  
  if (!buf->data) 
    {
      buf->allocated = 4096;
      buf->data = elfsh_malloc(4096);
      buf->maxlen = 0;
    }
  else
    if (buf->allocated  < (buf->maxlen + sizeof (elfshblk_t)))
    {
      buf->data = elfsh_realloc(buf->data, buf->allocated + 4096);
      buf->allocated += 4096;
    }
  
  is_func = 0;
  blen = sizeof (elfshblk_t);
  
  curblock = (elfshblk_t *) (buf->data + buf->maxlen);
  curblock->vaddr = cur->vaddr;
  curblock->contig = cur->contig;
  curblock->altern = cur->altern;
  curblock->size = cur->size;
  curblock->altype = cur->altype;

  snprintf(buffer, sizeof (buffer), "block_%08x", cur->vaddr);
    
  /*
    for each caller, add a new blcok reference to section
  */
  
  for (cal = cur->caller; cal; cal = cal->next) 
    {
      if (buf->allocated  < (buf->maxlen + blen + sizeof (elfshblkref_t)))
	{
	  buf->data = elfsh_realloc(buf->data, buf->allocated + 4096);
	  buf->allocated += 4096;
	}
      blockref = (elfshblkref_t *) (buf->data + buf->maxlen + blen);
      blockref->vaddr = cal->vaddr;
      blockref->type = cal->type;
      if (!is_func && cal->type == CALLER_CALL) 
	{
	  is_func = 1;
	  snprintf(buffer, sizeof (buffer), "function_%08x", cur->vaddr);
	} 
      // elfsh_append_data_to_section(sect, &blockref, sizeof (elfshblkref_t));
      blen += sizeof (elfshblkref_t);
      
    }
  
  bsym = elfsh_create_symbol(buf->maxlen, blen, STT_BLOCK, 0, 0, 0);
  elfsh_insert_symbol(buf->obj->secthash[ELFSH_SECTION_SYMTAB],
		      &bsym, buffer);

  buf->maxlen += blen;
  buf->block_counter++;
  return (0);
}

int			store_blocks(elfshobj_t *obj , struct s_iblock *blist, int mode) 
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  struct s_buf		buf;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  /*
    for each block, insert a new STT_BLOCK symbol
    and associated block in data of section
  */
    
  if (!(sect = elfsh_get_section_by_name(obj, ".control", 0, 0, 0)))
    {
      buf.allocated = 0;
      buf.maxlen = 0;
      buf.block_counter = 0;
      buf.data = 0;
      buf.obj = obj;
      if (mode)
	btree_browse_prefix(blist->btree, build_data, &buf);
      
      sect = elfsh_create_section(".control");
      shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, buf.maxlen,
			       0, 0, 0, 0);
      sect->altdata = blist;
      elfsh_insert_unmapped_section(obj, sect, shdr, buf.data);
    } 
  else
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
    "[elfsh:modflow] * \".control\" section already present\n", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.block_counter);
}

/**
 *
 *
 */

void	block_clean_passed(struct s_iblock *list) 
{
  struct s_iblock	*cur;
  for (cur = list; cur; cur = cur->next)
    cur->passed = 0;
}

/**
 * create a new block
 *
 */

struct s_iblock	*block_create(u_int vaddr, u_int size) 
{
  struct s_iblock	*t;
  
  t = elfsh_malloc(sizeof (struct s_iblock));
  memset(t, 0, sizeof (struct s_iblock));
  t->vaddr = vaddr;
  t->size = size;
  return (t);
};



/**
 * add a caller to a block.
 * sort by vaddr
 */
  
void	dump_block(struct s_iblock *b) 
{
  printf("[B]=(%08x) [V]=(%08x) sz=(%04u) [N]=(%08x)\n",
	 (int) b, b->vaddr, b->size, (int) b->next);
}


/*
 *
 *
 *
 */

int	apply_display(void *elem, void *opt)
{
  struct s_iblock	*cur;
  struct s_caller	*ccal;
  struct s_disopt	*disopt;
  char			*str;
  char			*end_str;
  elfsh_SAddr		offset;
  elfsh_SAddr		end_offset;
  char			buf1[30];
  char			buf2[30];
  

  cur = (struct s_iblock *) elem;
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

int	display_blocks(elfshobj_t *file, struct s_iblock *blk_list, 
		       int level) 
{
  struct s_disopt	disopt;
  
  disopt.counter = 0;
  disopt.level = level;
  disopt.file = file;
  if (blk_list->btree)
    btree_browse_infix(blk_list->btree, apply_display, &disopt);
  return (disopt.counter);
}


/**
 * add a new block to blocks linked list.
 * list is sorted by vaddr.
 * if block is already present, it's not inserted
 * and function return
 *
 */

void	block_add_list(struct s_iblock **list, struct s_iblock *n) 
{
  struct s_iblock	*cur;

  // printf("vaddr = %8x\n", n->vaddr);
  cur = *list;
  if (!cur)
    {
      cur = block_create(0, 0);
      *list = cur;
    }
  btree_insert_sort(&cur->btree, match_block, n);
}


  
/**
 * add a caller
 * - vaddr is address of starting block
 *
 */
void	block_add_caller(struct s_iblock *blk, u_int vaddr, int type) 
{
  struct s_caller	*n;
  
  n = elfsh_malloc(sizeof (struct s_caller));
  n->vaddr = vaddr;
  n->type = type;
  n->next = blk->caller;
  blk->caller = n;
}

/**
 * get a block by vaddr;
 * if mode is null, return blockonly if vaddr is
 * equal to block starting address
 * else return block if vaddr belong to block
 *
 */

struct s_iblock	*block_get_by_vaddr(struct s_iblock *list, u_int vaddr, 
				    int mode)
{
  struct s_iblock	cur;
  struct s_iblock	*elem;
  cur.vaddr = vaddr;

  if (list)
    {
      if (mode)
	elem = (struct s_iblock *) btree_find_elem(list->btree, 
						   match_inblock, &cur);
      else
	elem = (struct s_iblock *) btree_find_elem(list->btree, 
						   match_block, &cur);
    }
  else
    elem = 0;
  return (elem);
}


/**
 * this function add a new element to the linked list caller
 * of the current block
 * this function resolve operand of instruction which may
 * modifiy execution path.
 * if resolved to a virtual address, a new block is inserted
 * 
 * return destination address inserted or -1 if unresolved
 */
int	insert_destination_address(elfshobj_t *obj, 
				   asm_instr *ins, u_int vaddr,
				   struct s_iblock **blk_list) 
{
  int		ilen;
  int		dest;
  struct s_iblock	*dst;
  struct s_iblock	*dst_end;
  int			new_size;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  dest = 0;

  if ((ins->op1.content & ASM_OP_VALUE) && 
      !(ins->op1.content & ASM_OP_REFERENCE)) 
    {    
      ilen = asm_instr_len(ins);
      asm_operand_get_immediate(ins, 1, 0, &dest);
      dest += ilen + vaddr;

      /* 
	 search if destination is inside a block 
	 if not, create a new block of size 1
	 located at dst.
	 add block immediatly.
      */
      dst_end = *blk_list;
      dst = block_get_by_vaddr(dst_end, dest, 1);
      if (!dst) 
	{
	  dst = block_create(dest, 1);
	  dst->altype = CALLER_UNKN;
	  block_add_list(blk_list, dst);
	} 
    
      /* 
	 we have to truncate block found unless destination
	 address is block starting address in 2 new blocks:
	 ,--------------------.
	 |31|c0|5b|5e|5f|c9|c3|       xor %eax, %eax
	 ^     &                      pop %ebx, pop %esi, pop %edi, leave, ret
	 |     ^-- dest point there.
	 '- size -> 7. cut block at &
	 new_size may not be superior to previous size.
	 dst -> dst_end
      */
      else if (dst->vaddr != dest) 
	{
	  new_size = dst->size - (dest - dst->vaddr);
	  dst->size -= new_size;
	  dst_end = block_create(dest, new_size);
	  dst_end->contig = dst->contig;
	  dst_end->altern = dst->altern;
	  dst_end->altype = dst->altype;
	  dst->contig = dest;
	  dst->altype = CALLER_CONT;
	  dst->altern = 0;
	  block_add_list(blk_list, dst_end);
	  dst = dst_end;
	}

    
      block_add_caller(dst, vaddr, 
		       (ins->instr == ASM_CALL ? CALLER_CALL : CALLER_JUMP));

      /*
       * insert new block into symbol table
       */
      /*
      if (ins->instr == ASM_CALL) 
	{
	  sprintf(buffer, "function_%08x", dest);
	  newsym = elfsh_create_symbol(dest, 0, STT_FUNC, 0, 0, 0);
	  elfsh_insert_symbol(obj->secthash[ELFSH_SECTION_SYMTAB],
			      &newsym, buffer);
	}

      sprintf(buffer, "%s_%08x", ins->proc->instr_table[ins->instr], vaddr);
      newsym = elfsh_create_symbol(dest, -1, STT_BLOCK, 0, 0, 0);
      if ((0 > elfsh_insert_symbol(obj->secthash[ELFSH_SECTION_SYMTAB], 
				   &newsym, buffer)))
	printf("Insertion failed\n");
      */
    }

  /* 
   y2k6 <thorolf@grid.einherjar.de>
   handle something like this
   call  *%eax 
   check goto table and fixup destination
   - start gdb, set breakpoint on any "pointer function at [%x]
   - check register value
   - start elfsh
   - load modflow
   - addgoto <vaddr of call *%eax> <daddr value of *%eax>   
  */

  else if (ins->op1.content & ASM_OP_BASE) {
   char tmp[255];
   char *ret;
   printf("[***] pointer function at [%x]\n",vaddr);
   snprintf(tmp,250,"%x",vaddr);
   dst_end = *blk_list;

   if (mf_settings.rec_ptr_fnc &&
       vaddr_hist[4].instr == ASM_CALL &&
	   vaddr_hist[3].instr == ASM_MOV &&
  	   vaddr_hist[2].instr == ASM_MOV) {
	dest = vaddr_hist[2].op2.imm;

	if ((dest < elfsh_get_entrypoint(elfsh_get_hdr(world.curjob->current))) || (dest < 0)) {return -1;}	
     printf("[!!!] 0x%08x possible simple ptr function at [%x/%d] detected\n",vaddr,dest,dest);

  	 dst = block_get_by_vaddr(dst_end, dest, 1);

      if (!dst) 
	{
	  dst = block_create(dest, 1);
	  dst->altype = CALLER_UNKN;
	  block_add_list(blk_list, dst);
	} 
      else if (dst->vaddr != dest) 
	{
	  new_size = dst->size - (dest - dst->vaddr);
	  dst->size -= new_size;
	  dst_end = block_create(dest, new_size);
	  dst_end->contig = dst->contig;
	  dst_end->altern = dst->altern;
	  dst_end->altype = dst->altype;
	  dst->contig = dest;
	  dst->altype = CALLER_CONT;
	  dst->altern = 0;
	  block_add_list(blk_list, dst_end);
	  dst = dst_end;
	}

      block_add_caller(dst, vaddr, 
		       (ins->instr == ASM_CALL ? CALLER_CALL : CALLER_JUMP));

   } else {
    ret = (char *)hash_get(s_goto,tmp);
    if (ret) {
     dest = strtol(ret,(char **)NULL,16);
     if (dest) {
      printf("[***] extended routing table found 0x%08x -> 0x%08x\n",vaddr,dest);
      dst = block_get_by_vaddr(dst_end, dest, 1);

      if (!dst) 
	{
	  dst = block_create(dest, 1);
	  dst->altype = CALLER_UNKN;
	  block_add_list(blk_list, dst);
	} 
      else if (dst->vaddr != dest) 
	{
	  new_size = dst->size - (dest - dst->vaddr);
	  dst->size -= new_size;
	  dst_end = block_create(dest, new_size);
	  dst_end->contig = dst->contig;
	  dst_end->altern = dst->altern;
	  dst_end->altype = dst->altype;
	  dst->contig = dest;
	  dst->altype = CALLER_CONT;
	  dst->altern = 0;
	  block_add_list(blk_list, dst_end);
	  dst = dst_end;
	}

      block_add_caller(dst, vaddr, 
		       (ins->instr == ASM_CALL ? CALLER_CALL : CALLER_JUMP));
     }
    }
   }
  }
  else
    dest = -1;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}

