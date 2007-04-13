/*
** $Id: op_out_dx_al.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_out_dx_al" opcode="0xee"/>
 */

int     op_out_dx_al(asm_instr *new, u_char *opcode, u_int len,
                         asm_processor *proc) 
{
  new->len += 1;
    new->ptr_instr = opcode;
  new->instr = ASM_OUT;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, proc);
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_DX;
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_FIXED, proc);
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.base_reg = ASM_REG_AL;
#else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_DX;
  
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.base_reg = ASM_REG_AL;
#endif
  return (new->len);
}
