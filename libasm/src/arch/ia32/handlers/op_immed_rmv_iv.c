/*
** $Id: op_immed_rmv_iv.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_immed_rmv_iv" opcode="0x81"/>
 */

int op_immed_rmv_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  new->ptr_instr = opcode;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r) {
    case 0:
      new->instr = ASM_ADD;
      break;
    case 1:
      new->instr = ASM_OR;
      break;
    case 2:
      new->instr = ASM_ADC;
      break;
    case 3:
      new->instr = ASM_SBB;
      break;
    case 4:
      new->instr = ASM_AND;
      break;
    case 5:
      new->instr = ASM_SUB;
      break;
    case 6:
      new->instr = ASM_XOR;
      break;
    case 7:
      new->instr = ASM_CMP;
      break;
  }
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  operand_rmv_iv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
