/*
** $Id: op_shift_rmb_1.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shift_rmb_1" opcode="0xd0"/>
 */

int op_shift_rmb_1(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_SHIFT;
  new->ptr_instr = opcode;
  new->len += 1;
  

  new->instr = ASM_SHR;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, proc);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmb(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
  #endif
  return (new->len);
}
