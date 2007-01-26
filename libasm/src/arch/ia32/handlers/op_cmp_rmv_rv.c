/*
** $Id: op_cmp_rmv_rv.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmp_rmv_rv" opcode="0x39"/>
*/

int op_cmp_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CMP;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
