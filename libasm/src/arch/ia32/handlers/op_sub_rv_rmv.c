/*
** $Id: op_sub_rv_rmv.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sub_rv_rmv" opcode="0x2b"/>
*/

int op_sub_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_SUB;
  new->type = ASM_TYPE_ARITH;
  new->ptr_instr = opcode;
  
#if #LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_GENERAL, proc);
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_ENCODED, proc);
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
