/*
** $Id: op_shift_rmv_cl.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shift_rmv_cl" opcode="0xd3"/>
*/

int op_shift_rmv_cl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
    switch(modrm->r) {
    case 0:
      new->instr = ASM_ROL;
      break;
    case 5:
      new->instr = ASM_SHR;
      break;
    case 4:
      new->instr = ASM_SHL;
      break;
    case 7:
      new->instr = ASM_SAR;
      break;
    default:
      new->instr = ASM_BAD;
    }
    
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op2.type = ASM_OTYPE_FIXED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);

    new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op2.regset = ASM_REGSET_R8;
    new->op2.base_reg = ASM_REG_CL;
    new->op2.len = 0;
    new->op2.ptr = 0;
    
    new->len += new->op1.len + new->op2.len;
  return (new->len);
}
