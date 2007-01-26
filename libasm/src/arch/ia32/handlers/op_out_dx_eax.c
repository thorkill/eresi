/*
** $Id: op_out_dx_eax.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_out_dx_eax" opcode="0xef"/>
 */

int     op_out_dx_eax(asm_instr *new, u_char *opcode, u_int len,
                         asm_processor *proc) {
  new->len += 1;
    new->ptr_instr = opcode;
  new->instr = ASM_OUT;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_DX;
  
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.base_reg = ASM_REG_EAX;
  
  return (new->len);
}
