/*
** $Id: op_cmp_rb_rmb.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmp_rb_rmb" opcode="0x3a"/>
*/

int op_cmp_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TEST | ASM_TYPE_CONTROL;
  new->instr = ASM_CMP;
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERALBYTE, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODEDBYTE, proc);
  #else

  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;  
  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  #endif
  return (new->len);
}
