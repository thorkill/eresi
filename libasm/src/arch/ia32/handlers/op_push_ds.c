/*
** $Id: op_push_ds.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_push_ds" opcode="0x1e"/>
*/

int op_push_ds(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSH;
  // new->type = IS_MEM_WRITE;
  new->op1.type = ASM_OTYPE_FIXED;
  
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = ASM_REG_DS;
  return (new->len);
}
