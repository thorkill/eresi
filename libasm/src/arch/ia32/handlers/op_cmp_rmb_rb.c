/*
** $Id: op_cmp_rmb_rb.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmp_rmb_rb" opcode="0x38"/>
*/

int op_cmp_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_CMP;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
}
