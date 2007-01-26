/*
** $Id: op_push_cs.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0e
  Instruction :         PUSH
  */

int op_push_cs(asm_instr *new, u_char *opcode, u_int len,
               asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSH;
  
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = ASM_REG_CS;
  return (new->len);
}
