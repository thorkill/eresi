/**
 * @file op_mov_ref_iv_al.c
 * $Id: op_mov_ref_iv_al.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 * <instruction func="op_mov_ref_iv_al" opcode="0xa2"/>
 */

int op_mov_ref_iv_al(asm_instr *new, u_char *opcode, u_int len, 
		     asm_processor *proc) 
{
  new->instr = ASM_MOV;
  new->type = ASM_TYPE_ASSIGN;
  new->ptr_instr = opcode;
  new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_OFFSET, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_FIXED, new);
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.content = ASM_OP_BASE;
  new->op2.base_reg = ASM_REG_AL;
#else
  new->op1.type = ASM_OTYPE_OFFSET;
  new->op1.len = 4;
  new->op1.content = ASM_OP_VALUE | ASM_OP_REFERENCE;
  memcpy(&new->op1.imm, opcode + 1, 4);
  
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.content = ASM_OP_BASE;
  new->op2.base_reg = ASM_REG_AL;
  new->len += 4;
#endif
  return (new->len);
}
