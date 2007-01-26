/*
** $Id: op_mov_ref_iv_al.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_ref_iv_al" opcode="0xa2"/>
*/

int op_mov_ref_iv_al(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_MOV;
    new->ptr_instr = opcode;
  new->len += 5;
  
  new->op1.type = ASM_OTYPE_OFFSET;
  new->op1.len = 4;
  new->op1.content = ASM_OP_VALUE | ASM_OP_REFERENCE;
  memcpy(&new->op1.imm, opcode + 1, 4);
  
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.content = ASM_OP_BASE;
  new->op2.base_reg = ASM_REG_AL;
  return (new->len);
}
