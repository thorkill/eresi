/*
** $Id: op_out_ref_ib_al.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_out_ref_ib_al" opcode="0xe6"/>
*/


int op_out_ref_ib_al(asm_instr *new, u_char *opcode, u_int len, 
		     asm_processor *proc)
{
  new->instr = ASM_OUT;
  new->ptr_instr = opcode;
  new->len += 1;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, 
				ASM_OTYPE_IMMEDIATEBYTE, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_FIXED, new);
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.base_reg = ASM_REG_AL;
#else
    new->op1.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_VALUE;

  new->op1.imm = 0;
  memcpy(&new->op1.imm, opcode + 1, 1);
  new->len += 1;

  new->op2.type = ASM_OTYPE_FIXED;
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.base_reg = ASM_REG_AL;
#endif
  return (new->len);
}
