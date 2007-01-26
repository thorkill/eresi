/*
** $Id: op_imul_rv_rmv_iv.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_imul_rv_rmv_iv" opcode="0x69"/>
 */

int     op_imul_rv_rmv_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->instr = ASM_IMUL;
  new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  
  new->op3.type = ASM_OTYPE_IMMEDIATE;
  new->op3.ptr = opcode + new->len;
  new->op3.content = ASM_OP_VALUE;
  memcpy(&new->op3.imm, new->op3.ptr, 4);
  new->op3.len = 4;
  new->len += 4;
  return (new->len);
}
