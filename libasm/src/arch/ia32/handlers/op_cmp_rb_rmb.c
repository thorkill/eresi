/*
** $Id: op_cmp_rb_rmb.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmp_rb_rmb" opcode="0x3a"/>
*/

int op_cmp_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CMP;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  
  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}
