/*
** $Id: i386_mov_cr_rm.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int i386_mov_cr_rm(asm_instr *new, unsigned char *opcode, unsigned int len,
		   asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) (opcode + 1);
  new->len += 1;
  new->instr = ASM_MOV;

#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_CONTROL,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_CONTROL,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_REGISTER,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_REGISTER,				new);
#endif
#else
  new->op[0].type = ASM_OTYPE_CONTROL;
  new->op[0].content = ASM_OP_BASE;
  new->op[0].regset = ASM_REGSET_CREG;
  new->op[0].baser = modrm->r;

  new->op[1].type = ASM_OTYPE_REGISTER;
  new->op[1].content = ASM_OP_BASE;
  new->op[1].regset = ASM_REGSET_R32;
  new->op[1].baser = modrm->m;
#endif
  return (new->len);
}
