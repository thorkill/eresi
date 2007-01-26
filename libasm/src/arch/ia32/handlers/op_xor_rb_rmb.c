/*
** $Id: op_xor_rb_rmb.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_xor_rb_rmb" opcode="0x32"/>
*/

int op_xor_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_XOR;
  new->len += 1;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OSIZE_BYTE;
  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}
