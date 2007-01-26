/*
** $Id: op_or_rmb_rb.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x08
  Instruction :         OR
*/

int op_or_rmb_rb(asm_instr *new, u_char *opcode, u_int len,
                 asm_processor *proc) {
  new->instr = ASM_OR;
  new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op1.size = new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
}
