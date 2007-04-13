/*
** $Id: op_group6.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int     op_group6(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  new->len += 1;
  modrm = (struct s_modrm *) opcode + 1;
  switch(modrm->r) {
  case 0:
    new->instr = ASM_SLDT;
    break;
  case 1:
    new->instr = ASM_STR;
    break;
  case 2:
    new->instr = ASM_LLDT;
    break;
  case 3:
    new->instr = ASM_LTR;
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    break;
  }

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				proc);
  new->op1.regset = ASM_REGSET_R16;
  #else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
  new->op1.regset = ASM_REGSET_R16;
#endif
  return (new->len);
}
