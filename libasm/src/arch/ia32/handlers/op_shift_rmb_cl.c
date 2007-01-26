/*
** $Id: op_shift_rmb_cl.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shift_rmb_cl" opcode="0xd2"/>
 */

int op_shift_rmb_cl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
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
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmb(&new->op1, opcode + 1, len - 1, proc);
    new->op2.content = ASM_OP_BASE;
    new->op2.regset = ASM_REGSET_R8;
    new->op2.base_reg = ASM_REG_CL;
    new->len += new->op1.len;
  return (new->len);
}
