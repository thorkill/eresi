/*
** $Id: op_sbb_rv_rmv.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
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
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode +1, ASM_OTYPE_GENERAL, proc);
  new->len += asm_operand_fetch(&new->op2, opcode +1, ASM_OTYPE_ENCODED, proc);
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = new->op1.size = ASM_OSIZE_VECTOR;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
