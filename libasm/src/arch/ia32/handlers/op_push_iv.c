/*
** $Id: op_push_iv.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_push_iv" opcode="0x68"/>
*/

int op_push_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_PUSH;
  new->len += 5;
  new->ptr_instr = opcode;
  
  new->op1.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_VALUE;
  new->op1.ptr = opcode + 1;
  new->op1.len = 4;
  
  memcpy((char *) &new->op1.imm, opcode + 1, 4);
  
  return (new->len);
}
