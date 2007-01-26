/*
** $Id: op_btr_rmv_rv.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="op_btr_rmv_rv" opcode="0xb3"/>
*/

int     op_btr_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_BTR;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  
  return (new->len);
}
