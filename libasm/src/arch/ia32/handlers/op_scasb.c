/*
** $Id: op_scasb.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_scasb" opcode="0xae"/>
*/

int op_scasb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->instr = ASM_SCASB;
  new->ptr_instr = opcode;
  new->len += 1;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_YDEST;
  
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.prefix = ASM_PREFIX_ES;
  new->op2.base_reg = ASM_REG_EDI;
  
  
  return (new->len);
}
