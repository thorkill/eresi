/*
** $Id: op_shift_rmb_1.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
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
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmb(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
  // new->op1 = asm_create_operand(0, 0, 0, 0, proc);
  
  // new->op2 = asm_create_operand(0, 0, 0, 0, proc);
  // new->op2.content = OP_IMM_BYTE;
  
  return (new->len);
}
