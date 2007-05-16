/*
** $Id: op_cmp_rmv_rv.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmp_rmv_rv" opcode="0x39"/>
*/

int op_cmp_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->type = ASM_TYPE_TEST | ASM_TYPE_CONTROL;
  new->ptr_instr = opcode;
  new->instr = ASM_CMP;

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERAL, proc);
  #else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;  
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  #endif
  return (new->len);
}
