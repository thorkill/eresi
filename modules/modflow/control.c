/*
** 
** sk_onctrol.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Thu May 29 20:44:39 2003
** Updated : Sun Oct 19 16:45:48 2003
*/

#include "modflow.h"

#define BIN_LINUX	0
#define BIN_FREEBSD	1

/**
 * this function get last push argument
 * until a call is finded.
 * this is some crap stuff to fetch main address
 */

u_int			trace_start(elfshobj_t *obj, char *buf, 
				    u_int len, u_int vaddr,
				    struct s_iblock **b_lst, 
				    struct s_function **f_lst) 
{
  int			stop;
  elfsh_Sym		*sym;
  u_int			ilen;
  u_int			dis;
  u_int			main_addr;
  u_int			init_addr;
  struct s_iblock	*main_b;
  asm_instr		ins;
  int			arch_bin, fetch_next;

  fetch_next = 0;
  if (!(elfsh_get_objtype(elfsh_get_hdr(obj)) == ET_EXEC))
    return 0;
  printf(" [*] _start found at %08x\n", vaddr);
  // function_add_list(f_lst, function_create(vaddr, "_start"));

  for (dis = stop = 0; !stop; dis += ilen) 
    {

      ilen = asm_read_instr(&ins, (u_char *)buf + dis, len - dis, &world.proc);

      if (!dis) {
	switch(ins.instr) 
	  {
	  case ASM_XOR:      
	    arch_bin = BIN_LINUX;
	    break;
	  case ASM_PUSH:
	    arch_bin = BIN_FREEBSD;
	    sym = elfsh_get_metasym_by_name(obj, ".init");
	    init_addr = sym->st_value;
	    printf(" [*] locating call to .init: %8x\n", init_addr);
	    break;
	  }
	printf(" [*] %s-like start\n", arch_bin ? "FreeBSD" : "Linux");  
      }
      switch(ins.instr) 
	{
	case ASM_PUSH:
	  if (dis && (arch_bin == BIN_LINUX) && 
	      ins.op1.type == ASM_OTYPE_IMMEDIATE)
	    asm_operand_get_immediate(&ins, 1, 0, &main_addr);
	  break;
	case ASM_CALL:
	  if (arch_bin == BIN_FREEBSD) 
	    {
	      asm_operand_get_immediate(&ins, 1, 0, &main_addr);
	      if (fetch_next)
		stop = 1;
	      main_addr += vaddr + dis + ilen;
	      if (main_addr == init_addr)
		fetch_next = 1;
	      break;	
	    }
	  if ((arch_bin == BIN_LINUX)) 
	    {
	      stop = 1;
	      break;
	    }
	}

    }

  // function_add_list(f_lst, function_create(main_addr, "main"));
  block_add_list(b_lst, main_b = block_create(main_addr, 1));
  block_add_caller(main_b, vaddr + dis, CALLER_CALL);
  return (main_addr);
}


/**
 *
 * this function trace execution flow and creates
 * block depending on instruction.
 * if instruction break execution flow, block is
 * considerated finished and added to linked list of blocks
 * 2 static variables are used to keep state of execution flow
 *
 */

static struct s_iblock	*g_curblock = 0;	/* current working block	*/
u_int			g_prevaddr = 0;	/* previous instruction address		*/
asm_instr		g_previns;	/* previous instruction if CMP/TEST	*/

void			trace_control(elfshobj_t *obj, 
				      asm_instr *ins, u_int vaddr,
				      struct s_iblock **blk_list) 
{
  int			ilen;
  struct s_iblock	*tmp;
  
  tmp = 0;
  if (!g_curblock) 
    {

      /* previous block is finished
	 search current block by vaddr if previously created	
	 if found, set size to 0 as we are going to disassemble it
	 if not found, create a new block unless instruction is a NOP
	 -this is to escape some padding in begining of .text section-
	 if (g_prevaddr is not null, then, new block)
	 */
      if ((g_curblock = block_get_by_vaddr(*blk_list, vaddr, 0)))
	g_curblock->size = 0;
      else 
	{
	  g_curblock = block_create(vaddr, 0);
	  if (g_prevaddr)
	    block_add_caller(g_curblock, g_prevaddr, CALLER_CONT);
	}
    }
  
  /* 
     If previous block is not finished, search for a block
     which begin at current virtual addresss.
     if found, then, current instruction is the first of a previously
     detected block.
     NOTE: this may help in obfuscated code detection
     */
  else if ((tmp = block_get_by_vaddr(*blk_list, vaddr, 0))) 
    {
      // printf(" [d3bUg@!#$!] found %08x\n", vaddr);
      g_curblock->altype = CALLER_CONT;
      g_curblock->contig = vaddr;
      block_add_list(blk_list, g_curblock);
      g_curblock = tmp;
      g_curblock->size = 0;
      g_curblock = tmp;
    }
  
  /*
    from there, we MUST be in a block
    assert if not.
    */
  if (!g_curblock) 
    {
      /* printf("%08x: block not found and creation failure... exiting\n", vaddr); */
      exit(-1);
    } 

  /*
    keep track of current address
    add current instruction size to block size.
    */
  g_prevaddr = vaddr;
  ilen = asm_instr_len(ins);
  g_curblock->size += ilen;
  memcpy(&g_previns, ins, sizeof (asm_instr));
  
  if (!tmp)
    block_add_list(blk_list, g_curblock);
  
  /*
    depending on instruction type -based on IA32 instruction set-
    ASM_TYPE_CONDBRANCH: jcc, loop, MAY not break execution flow
    ASM_TYPE_CALLPROC: calls break instruction flow but CAN restore it
    ASM_TYPE_IMPBRANCH, ASM_TYPE_RETPROC: jmp and ret break execution flow
    last two type reset g_prevaddr as execution flow won't be restored
    to following instruction
    */
  
  switch (ins->type) 
    {
    case ASM_TYPE_CONDBRANCH:
      g_curblock->contig = vaddr + ilen;
      g_curblock->altern = 
	insert_destination_address(obj, ins, vaddr, blk_list);
      g_curblock->altype = CALLER_JUMP;
      g_curblock = 0;
      break;
    case ASM_TYPE_CALLPROC:
      g_curblock->contig = vaddr + ilen;
      g_curblock->altern = 
	insert_destination_address(obj, ins, vaddr, blk_list);
      g_curblock->altype = CALLER_CALL;
      g_curblock = 0;
      break;
    case ASM_TYPE_IMPBRANCH:
      g_curblock->altern = 
	insert_destination_address(obj, ins, vaddr, blk_list);
      g_curblock->altype = CALLER_JUMP;
      g_curblock = 0;
      g_prevaddr = 0;
      break;
    case ASM_TYPE_RETPROC:
      g_curblock->contig = 0;
      g_curblock->altern = 0;
      g_curblock->altype = CALLER_RET;
      g_curblock = 0;
      g_prevaddr = 0;
      break;
    }
  
}
