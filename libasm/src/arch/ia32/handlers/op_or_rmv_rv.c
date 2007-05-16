/*
** $Id: op_or_rmv_rv.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
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
  new->type = ASM_TYPE_ARITH;
  new->ptr_instr = opcode;
  new->instr = ASM_OR;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERAL, proc);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
