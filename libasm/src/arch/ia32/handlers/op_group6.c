/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

int     op_group6(asm_instr *new, u_char *opcode, u_int len,
		  asm_processor *proc)
{
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
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				new);
#endif
  new->op[0].regset = ASM_REGSET_R16;
  #else
  new->op[0].content = ASM_CONTENT_ENCODED;
  new->op[0].size = ASM_OSIZE_VECTOR;
  operand_rmv(&new->op[0], opcode + 1, len - 1, proc);
  new->len += new->op[0].len;
  new->op[0].regset = ASM_REGSET_R16;
#endif
  return (new->len);
}
