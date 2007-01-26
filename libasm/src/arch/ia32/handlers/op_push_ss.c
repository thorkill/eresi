/*
** $Id: op_push_ss.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_push_ss" opcode="0x16"/>
*/

int     op_push_ss(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_PUSH;
  new->len += 1;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_FIXED;
  
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_SREG;
  // new->type = IS_MEM_WRITE;
  return (new->len);
}
