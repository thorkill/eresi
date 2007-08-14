/**
 * @file op_mov_ref_iv_al.c
 * @ingroup handlers_ia32
 * $Id: op_mov_ref_iv_al.c,v 1.6 2007-08-14 06:52:55 strauss Exp $
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

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_OFFSET, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_FIXED, new);
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.content = ASM_OP_BASE;
  new->op2.baser = ASM_REG_AL;

  return (new->len);
}
