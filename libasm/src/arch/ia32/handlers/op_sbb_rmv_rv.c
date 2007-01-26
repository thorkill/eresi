/*
** $Id: op_sbb_rmv_rv.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sbb_rmv_rv" opcode="0x19"/>
*/

int op_sbb_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_SBB;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = new->op1.size = ASM_OSIZE_VECTOR;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
