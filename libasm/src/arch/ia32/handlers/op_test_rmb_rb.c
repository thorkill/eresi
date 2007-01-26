/*
** $Id: op_test_rmb_rb.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_test_rmb_rb" opcode="0x84"/>
*/


int op_test_rmb_rb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc){
  new->len += 1;
  new->instr = ASM_TEST;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
  return (new->len);
}
