/*
** $Id: op_or_rv_rmv.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0b
  Instruction :         OR
*/

int op_or_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_OR;
  new->ptr_instr = opcode;
  new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, proc);
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
