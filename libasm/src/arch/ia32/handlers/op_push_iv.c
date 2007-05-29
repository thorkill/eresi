/**
 * @file o_push_iv.c
 * $Id: op_push_iv.c,v 1.4 2007-05-29 00:40:28 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 * Handler for opcode 0x68 , instruction push_iv
  <instruction func="op_push_iv" opcode="0x68"/>
*/

int op_push_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_PUSH;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TOUCHSP;
  new->spdiff = -4;
  
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_IMMEDIATE, 
				new);
#else
  new->op1.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_VALUE;
  new->op1.ptr = opcode + 1;
  new->op1.len = 4;
  
  memcpy((char *) &new->op1.imm, opcode + 1, 4);
  new->len += 4;
#endif
  return (new->len);
}
