/*
** $Id: op_in_al_ref_ib.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_al_ref_ib" opcode="0xe4"/>
*/

int op_in_al_ref_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_IN;
  new->ptr_instr = opcode;
  new->len += 1;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_FIXED, proc);
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_AL;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_IMMEDIATEBYTE, proc);
#endif

  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_AL;

  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.content = ASM_OP_VALUE;

  new->len += 1;
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, 1);
  
  return (new->len);
}
