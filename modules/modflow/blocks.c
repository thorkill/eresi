/*
** 
** sk_blocks.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Thu May 29 20:39:14 2003
** Updated : Wed Jun 18 00:44:00 2003
*/

#include "modflow.h"

char	*call_type_str[] = 
{
  "CONT",
  "JUMP",
  "CALL",
  "RET",
  "UNKN"
};


int	block_is_called(struct s_iblock *blk) 
{
  struct s_caller	*cur;
  
  for (cur = blk->caller; cur; cur = cur->next) {
    if (cur->type == CALLER_CALL)
      return (1);
  }
  return (0);
}

/*
void	load_blocks()
{
  Elf32_Shdr		shdr;
  elfshsect_t		*sect;
  

}
*/

void	store_blocks(elfshobj_t *obj , struct s_iblock *blist) 
{
  Elf32_Shdr		shdr;
  elfshsect_t		*sect;
  Elf32_Sym		bsym;
  elfshblk_t		curblock;
  elfshblkref_t		blockref;
  struct s_iblock	*cur;
  struct s_caller	*cal;
  u_int		        blen;
  u_int			maxlen;
  int			is_func;
  char			buffer[24];
  char			*data;

  maxlen = 0;
  
  /*
    for each block, insert a new STT_BLOCK symbol
    and associated block in data of section
  */

  
  for (cur = blist; cur; cur = cur->next) 
    {
      if (!maxlen)
	data = malloc(sizeof (elfshblk_t));
      else
	data = realloc(data, maxlen + sizeof (elfshblk_t));

      /* dump_block(cur); */
      is_func = 0;
      blen = sizeof (elfshblk_t);
      curblock.vaddr = cur->vaddr;
      curblock.contig = cur->contig;
      curblock.altern = cur->altern;
      curblock.size = cur->size;
      curblock.altype = cur->altype;
      memcpy(data + maxlen, &curblock, sizeof (elfshblk_t));
      /*
      if (!maxlen)
      else
	elfsh_append_data_to_section(sect, &curblock, blen);
      */
      snprintf(buffer, sizeof (buffer), "block_%08x", cur->vaddr);

      /*
	for each caller, add a new blcok reference to section
      */
      
      for (cal = cur->caller; cal; cal = cal->next) 
	{
	  blockref.vaddr = cal->vaddr;
	  blockref.type = cal->type;
	  if (!is_func && cal->type == CALLER_CALL) 
	    {
	      is_func = 1;
	      snprintf(buffer, sizeof (buffer), "function_%08x", cur->vaddr);
	    }
	  data = realloc(data, maxlen + blen + sizeof (elfshblkref_t));
	memcpy(data + maxlen + blen, &blockref, sizeof (elfshblkref_t));
	  // elfsh_append_data_to_section(sect, &blockref, sizeof (elfshblkref_t));
	  blen += sizeof (elfshblkref_t);
	}
      
      /*
	create symbol
      */
      /* printf("maxlen = %i blen = %i\n buffer=%s\n", maxlen, blen, buffer); */
      
      bsym = elfsh_create_symbol(maxlen, blen, STT_BLOCK, 0, 0, 0);
      elfsh_insert_symbol(obj->secthash[ELFSH_SECTION_SYMTAB],
			    &bsym, buffer);
      
      maxlen += blen;
    }

  
  if (!(sect = elfsh_get_section_by_name(obj, ".control", 0, 0, 0)))
    {
      sect = elfsh_create_section(".control");
      shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, maxlen,
			       0, 0, 0, 0);
    }
  
  elfsh_insert_unmapped_section(obj, sect, shdr, data);
  sect->altdata = blist;
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
  
  t = malloc(sizeof (struct s_iblock));
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

int	display_blocks(elfshobj_t *file, struct s_iblock *blk_list, 
		       int level) 
{
  struct s_iblock	*cur;
  struct s_caller	*ccal;
  u_int			offset;
  char			*str;
  int			num;

  char			*end_str;
  u_int			end_offset;

  char			buf1[30];
  char			buf2[30];

  num = 0;
  for (cur = blk_list; cur; cur = cur->next) 
    {

      num++;
      str = elfsh_reverse_metasym(file, cur->vaddr, &offset);
      end_str = elfsh_reverse_metasym(file, cur->vaddr + cur->size, &end_offset);

      if (str == NULL)
	*buf1 = 0x00;
      else
	snprintf(buf1, sizeof (buf1), "<%s + %i>", str, offset);
      if (end_str == NULL || !(cur->contig))
	*buf2 = 0x00;
      else
	snprintf(buf2, sizeof (buf2), "<%s + %i>", end_str, end_offset);
      
      printf("[%08x:%05i:%08x:%08x] %-4s %-30s --> %-30s ", 
	     cur->vaddr, cur->size, cur->contig, cur->altern, 
	     call_type_str[cur->altype], buf1, buf2);
      
      if (cur->altern == 0xFFFFFFFF)
	printf(" [?]");
      else if (cur->altern != NULL)
	{
	  str = elfsh_reverse_metasym(file, cur->altern, &offset);
	  printf(" [%s + %i]", (str ? str : ""), offset);
	}
      
      printf("\n");
      if (level > 0)
	for (ccal = cur->caller; ccal; ccal = ccal->next) 
	  {
	    str = elfsh_reverse_metasym(file, ccal->vaddr, &offset);
	    printf("\texecuted from: (%08x) <%s + %i> : %s\n",
		   ccal->vaddr, (str ? str : ""), offset, 
		   call_type_str[ccal->type]);
	  }
    }
  return (num);
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
  struct s_iblock	*nxt;
  // char	buffer[12];

  /*
  printf("[@#$!#$ vaddr = %08x size = %3i] - ", n->vaddr, n->size);
  snprintf(buffer, sizeof (buffer), "%08x", n->vaddr);
  if ((cur = hash_get(&block_hash, buffer))) {
    puts("skipped");
    return;
  } else
    puts("added");
  */
  // hash_add(&block_hash, buffer, n);
  if ((cur = *list)) 
    {
      if (n->vaddr < cur->vaddr) 
	{
	  n->next = *list;
	  *list = n;
	  return;
	} 
      else 
	{
	  /*
	    we browse linked list until we find that vaddr of block to add
	    is greater or equal to current block vaddr and lesser than next 
	    block vaddr.
	    if  both vaddr are equal, block is already present in linked list
	    so we simply return. else, we add the new block between current 
	    and next block of linked list
	  */
	  while (cur->next) 
	    {
	      nxt = cur->next;
	      if ((n->vaddr >= cur->vaddr) && (n->vaddr < nxt->vaddr)) 
		{
		  if (n->vaddr != cur->vaddr)
		    break;
		  return;
		}
	      cur = cur->next;
	    } 
	  
	  /* 
	     new element position found :  
	     cur -> new - > cur->next
	  */
	  if (n->vaddr != cur->vaddr) 
	    {
	      n->next = cur->next;
	      cur->next = n;
	    }
	}

    } /* !*list is empty. */
  else 
    {
      n->next = *list;
      *list = n;
    }
}

/**
 * add a caller
 * - vaddr is address of starting block
 *
 */
void	block_add_caller(struct s_iblock *blk, u_int vaddr, int type) 
{
  struct s_caller	*n;
  
  n = malloc(sizeof (struct s_caller));
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
  struct s_iblock	*cur;
  char			buffer[12];
  
  snprintf(buffer, sizeof (buffer), "%08x", vaddr);
  if (!(cur = hash_get(&block_hash, buffer)))
    for (cur = list; cur; cur = cur->next) 
      {
	if (((cur->vaddr == vaddr)) || 
	    (mode && ((cur->vaddr < vaddr) && (vaddr < (cur->vaddr + cur->size)))))
	  break;
	else
	  if (vaddr <  (cur->vaddr + cur->size))
	    return (0);
      }
  return (cur);
}


/**
 * this function add a new element to the kinked list caller
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
  
  dest = 0;

  if ((ins->op1.content & ASM_OP_VALUE) && 
      !(ins->op1.content & ASM_OP_REFERENCE)) 
    {    
      ilen = asm_instr_len(ins);
      asm_operand_get_immediate(&ins->op1, &dest);
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
  else
    dest = -1;
  return (dest);
}


