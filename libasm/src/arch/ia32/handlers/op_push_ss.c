/*
** $Id: op_push_ss.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
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

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, proc);
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = ASM_REG_SS;
#else
  new->op1.type = ASM_OTYPE_FIXED;  
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_SREG;
  // new->type = IS_MEM_WRITE;
  #endif
  return (new->len);
}
