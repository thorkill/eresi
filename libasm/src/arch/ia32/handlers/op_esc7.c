/*
** $Id: op_esc7.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_esc7" opcode="0xdf"/>
*/

int op_esc7(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  if (*(opcode + 1) == 0xe0) {
    new->instr = ASM_FNSTSW;
    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.content = ASM_OP_BASE;
    new->op1.regset = ASM_REGSET_R16;
    new->op1.base_reg = ASM_REG_EAX;
  } else
    switch (modrm->r) {
    case 0:
      new->instr = ASM_FILD;
      break;
    case 1:
      // bad new->instr = ASM_;
      break;
    case 2:
      new->instr = ASM_FIST;
      break;
    case 3:
      new->instr = ASM_FISTP;
      break;
    case 4:
      new->instr = ASM_FBLD;
      break;
    case 5:
      new->instr = ASM_FILD;
      break;
    case 6:
      new->instr = ASM_FBSTP;
      break;
    case 7:
      new->instr = ASM_FISTP;
      break;
    }
  if (*(opcode + 1) != 0xe0) {
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len;
  } else
    new->len++;
  return (new->len);
}
