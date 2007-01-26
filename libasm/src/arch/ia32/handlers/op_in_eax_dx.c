/*
** $Id: op_in_eax_dx.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_eax_dx" opcode="0xed"/>
 */

int     op_in_eax_dx(asm_instr *new, u_char *opcode, u_int len,
                         asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_IN;
    new->ptr_instr = opcode;
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.regset = ASM_REGSET_R16;
  new->op2.base_reg = ASM_REG_DX;
  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  
  return (new->len);
}
