/*
** $Id: op_and_rmb_rb.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_and_rmb_rb" opcode="0x20"/>
*/

int op_and_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_AND;
  
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmb_rb(new, opcode + 1, len -1, proc);
  return (new->len);
}
