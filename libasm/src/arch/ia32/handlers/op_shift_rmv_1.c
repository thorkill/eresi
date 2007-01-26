/*
** $Id: op_shift_rmv_1.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shift_rmv_1" opcode="0xd1"/>
 */

int op_shift_rmv_1(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  switch(modrm->r) {
  case ASM_REG_EDI:
    new->instr = ASM_SAR;
    break;
  case ASM_REG_ESP:
    new->instr = ASM_SHL;
    break;
  default:
    new->instr = ASM_SHR;
    break;
  }
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  /*
    new->op1 = asm_create_operand(0, 0, 0, 0, proc);
    new->op1.len = 1;
    new->op1.content = OP_REG;
    new->op1.base_reg = modrm->m;
  */
  new->len += new->op1.len;
  return (new->len);
}
