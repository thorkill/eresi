/*
** $Id: op_or_rmv_rv.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x09
  Instruction :         OR
*/

int op_or_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_OR;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
