/**
 * @file i386_cmove.c
 * $Id: i386_cmove.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_cmove" opcode="0x44"/>
*/


int i386_cmove(asm_instr *new, u_char *opcode, u_int len, 
	       asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_CMOVE;
  
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, 
				new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, 
				new);    
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  #endif
  return (new->len);
}

