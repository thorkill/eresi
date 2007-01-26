/*
** $Id: op_in_al_dx.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_al_dx" opcode="0xec"/>
 */

int     op_in_al_dx(asm_instr *new, u_char *opcode, u_int len,
                         asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_IN;
  new->op1.type = ASM_OTYPE_FIXED;
    new->ptr_instr = opcode;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_AL;
  
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.regset = ASM_REGSET_R16;
  new->op2.base_reg = ASM_REG_DX;
  
  return (new->len);
}
