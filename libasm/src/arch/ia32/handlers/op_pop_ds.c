/*
** $Id: op_pop_ds.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_pop_ds" opcode="0x1f"/>
*/

int     op_pop_ds(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->instr = ASM_POP;
  new->ptr_instr = opcode;
  new->len += 1;
  // new->instr = IS_MEM_READ;
  new->op1.type = ASM_OTYPE_FIXED;
  
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = ASM_REG_DS;
  new->op1.len = 0;
  
  return (new->len);
}
