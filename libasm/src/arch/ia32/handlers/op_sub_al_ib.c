/*
** $Id: op_sub_al_ib.c,v 1.4 2007-05-29 00:40:28 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sub_al_ib" opcode="0x2c"/>
*/

int op_sub_al_ib(asm_instr *new, u_char *opcode, u_int len, 
		 asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_SUB;
  new->type = ASM_TYPE_ARITH;
  new->ptr_instr = opcode;
  
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_AL;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, 
				ASM_OTYPE_IMMEDIATEBYTE, new);
  #else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.base_reg = ASM_REG_AL;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = 1;
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, 1);
  new->len += 1;
  #endif
  return (new->len);
}
