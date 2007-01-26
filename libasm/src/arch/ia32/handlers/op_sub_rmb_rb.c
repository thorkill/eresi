/*
** $Id: op_sub_rmb_rb.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sub_rmb_rb" opcode="0x28"/>
*/



int op_sub_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_SUB;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->ptr_instr = opcode;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
}
