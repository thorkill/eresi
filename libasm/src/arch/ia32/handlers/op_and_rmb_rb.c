/*
** $Id: op_and_rmb_rb.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_and_rmb_rb" opcode="0x20"/>
*/

int op_and_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_ARITH;
  new->instr = ASM_AND;
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERALBYTE, proc);
  #else
  
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmb_rb(new, opcode + 1, len -1, proc);
#endif
  return (new->len);
}
