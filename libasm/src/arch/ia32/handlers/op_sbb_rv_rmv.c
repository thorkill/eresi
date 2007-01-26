/*
** $Id: op_sbb_rv_rmv.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sbb_rv_rmv" opcode="0x1b"/>
*/

int op_sbb_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->len += 1;
  new->instr = ASM_SBB;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = new->op1.size = ASM_OSIZE_VECTOR;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
