/*
** $Id: op_in_eax_dx.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_eax_dx" opcode="0xed"/>
 */

int     op_in_eax_dx(asm_instr *new, u_char *opcode, u_int len,
                         asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_IN;
  new->ptr_instr = opcode;
 
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.base_reg = ASM_REG_EAX;
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_FIXED, new);
  new->op2.regset = ASM_REGSET_R16;
  new->op2.base_reg = ASM_REG_DX;  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;

#else
 
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.base_reg = ASM_REG_EAX;
  
  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.regset = ASM_REGSET_R16;
  new->op2.base_reg = ASM_REG_DX;  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  #endif
  return (new->len);
}
