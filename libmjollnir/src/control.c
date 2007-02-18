/*
** control.c in libmjollnir for elfsh
**
** Started : Thu May 29 20:44:39 2003 sk
** Updated : Sun Dec 30 16:45:48 2006 mayhem
*/
#include "libmjollnir.h"



/* This function traces the entry point and save the last push argument until 
** a call is found. This allow to fetch the main address in an OS-dependent 
** manner */
elfsh_Addr	        mjr_trace_start(mjrcontext_t	*context,
					u_char		*buf, 
					u_int		len, 
					elfsh_Addr	vaddr)
{
  int			stop;
  elfsh_Sym		*sym;
  u_int			ilen;
  u_int			dis;
  elfsh_Addr		main_addr;
  elfsh_Addr		init_addr;
  mjrblock_t		*main_b;
  asm_instr		ins;
  int			arch_bin;
  int			fetch_next = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!context || !buf)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid parameters", 0);
  
  if (!(elfsh_get_objtype(elfsh_get_hdr(context->obj)) == ET_EXEC))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Object is not ET_EXEC", 0);

#if defined(__DEBUG_MJOLLNIR__)
  printf(" [*] _start found at %lx\n", (unsigned long) vaddr);
#endif
 
  for (dis = stop = 0; !stop; dis += ilen) 
    {
      ilen = asm_read_instr(&ins, (u_char *) buf + dis, len - dis, &context->proc);
      if (!dis) 
	{
	  switch (ins.instr) 
	    {
	    case ASM_XOR:      
	      arch_bin = MJR_BIN_LINUX;
	      break;
	    case ASM_PUSH:
	      arch_bin = MJR_BIN_FREEBSD;
	      sym = elfsh_get_metasym_by_name(context->obj, 
					      ELFSH_SECTION_NAME_INIT);
	      init_addr = sym->st_value;
	      printf(" [*] locating call to .init: %lx\n", 
		     (unsigned long) init_addr);
	      break;
	    }
	  printf(" [*] %s-like start\n", arch_bin ? "FreeBSD" : "Linux");  
	}
      
      switch (ins.instr) 
	{
	  
	  // XXX: This code is never executed as arch_bin = FreeBSD for a PUSH
	  // See 10 lines before that comment. Need to confirm removing.
	case ASM_PUSH:
	  if (dis && (arch_bin == MJR_BIN_LINUX) && 
	      ins.op1.type == ASM_OTYPE_IMMEDIATE)
	    asm_operand_get_immediate(&ins, 1, 0, &main_addr);
	  break;
	case ASM_CALL:
	  if (arch_bin == MJR_BIN_FREEBSD) 
	    {
	      asm_operand_get_immediate(&ins, 1, 0, &main_addr);
	      if (fetch_next)
		stop = 1;
	      main_addr += vaddr + dis + ilen;
	      if (main_addr == init_addr)
		fetch_next = 1;
	      break;	
	    }
	  if ((arch_bin == MJR_BIN_LINUX)) 
	    {
	      stop = 1;
	      break;
	    }
	}
    }

  main_b = mjr_block_create(context, main_addr, 1);
 

  mjr_block_add_list(context, main_b);
  mjr_block_add_caller(main_b, vaddr + dis, CALLER_CALL);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, main_addr);
}


/*
** This function trace execution flow and creates block depending on instruction.
**
** If instruction break execution flow, block is considerated finished and added 
** to linked list of blocks (that is the content of the .elfsh.control section)
**
*/
int			mjr_trace_control(mjrcontext_t	*context,
					  elfshobj_t    *obj, 
					  asm_instr     *ins, 
					  elfsh_Addr	vaddr)
{
  int			ilen;
  int			ret;
  mjrblock_t		*tmp = NULL;
  
  /* Initialy enter here */
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If last instr was a jmp/call/ret, or if we are at entry point,
  ** curblock will be NULL here */
  if (!context->curblock) 
    {

      /* previous block is finished
      ** - search current block by vaddr
      ** - if found, set size to 0 as we are going to disassemble it
      ** - if not found, create a new block unless instruction is a NOP
      ** -> this is to escape some padding
      ** -> if prevaddr is not null, then we create a new block
      */
      if ((context->curblock = mjr_block_get_by_vaddr(context, vaddr, 0)))
	context->curblock->size = 0;
      else 
	{
	  context->curblock = mjr_block_create(context, vaddr, 0);
	  if (context->hist[MJR_HISTORY_PREV].vaddr)
	    mjr_block_add_caller(context->curblock, 
				 context->hist[MJR_HISTORY_PREV].vaddr,
				 CALLER_CONT);
	}
   }
  
  /* 
  ** If previous block is not finished, search for a block which begin at current
  ** virtual address. If found, then the current instruction is the first of a 
  ** previously detected block. 
  */
  else if ((tmp = mjr_block_get_by_vaddr(context, vaddr, 0))) 
    {
      context->curblock->type  = CALLER_CONT;
      context->curblock->true  = vaddr;
      mjr_block_add_list(context, context->curblock);
      context->curblock->size  = 0;
      context->curblock        = tmp;
    }
  
  /* From there, we MUST be in a block */
  assert(context->curblock != NULL);

  /* Keep track of curaddr and add current instruction size to block size */
  context->curblock->size += ilen;
  mjr_history_write(context, ins, vaddr, MJR_HISTORY_CUR);
  if (!tmp)
    mjr_block_add_list(context, context->curblock);
  
  /* Now abstract interpret the operational semantics of the current 
     instruction set using the typed instruction system of libasm */
  context->obj      = obj;
  mjr_history_write(context, ins, vaddr, 0);
  ret = mjr_asm_flow(context);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to interpret instruction type", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
