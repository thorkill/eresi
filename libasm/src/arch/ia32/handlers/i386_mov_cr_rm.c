/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

int i386_mov_cr_rm(asm_instr *new, u_char *opcode, u_int len,
		   asm_processor *proc)
{
  struct s_modrm *modrm;

  modrm = (struct s_modrm *) (opcode + 1);
  new->len += 1;
  new->instr = ASM_MOV;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_CONTROL, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_REGISTER, new);
#else
  new->op[0].content = ASM_CONTENT_CONTROL;
  new->op[0].type = ASM_OPTYPE_REG;
  new->op[0].regset = ASM_REGSET_CREG;
  new->op[0].baser = modrm->r;

  new->op[1].content = ASM_CONTENT_REGISTER;
  new->op[1].type = ASM_OPTYPE_REG;
  new->op[1].regset = ASM_REGSET_R32;
  new->op[1].baser = modrm->m;
#endif
  return (new->len);
}
