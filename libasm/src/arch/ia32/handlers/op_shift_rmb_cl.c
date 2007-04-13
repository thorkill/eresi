/*
** $Id: op_shift_rmb_cl.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shift_rmb_cl" opcode="0xd2"/>
 */

int op_shift_rmb_cl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  switch(modrm->r) {
  case 1:
    new->instr = ASM_ROR;
    break;
  default:
    new->instr = ASM_SHIFT;
  }

#if LIBASM_USE_OPERAND_VECTOR
  new->len = asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, proc);
  new->len = asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_FIXED, proc);
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.base_reg = ASM_REG_CL;
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmb(&new->op1, opcode + 1, len - 1, proc);
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.base_reg = ASM_REG_CL;
  new->len += new->op1.len;
#endif
  return (new->len);
}
