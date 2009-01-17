/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

int i386_mov_rm_cr(asm_instr *new, u_char *opcode, u_int len,
		   asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) (opcode + 1);
  new->len += 1;

#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_REGISTER,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_REGISTER,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_CONTROL,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_CONTROL,				new);
#endif
#else

    new->instr = ASM_MOV;
    new->op[0].content = ASM_CONTENT_REGISTER;
    new->op[0].type = ASM_OP_BASE;
    new->op[0].regset = ASM_REGSET_R32;
    new->op[0].baser = modrm->m;
    new->op[1].content = ASM_CONTENT_SEGMENT;
    new->op[1].type = ASM_OP_BASE;
    new->op[1].regset = ASM_REGSET_CREG;
    new->op[1].baser = modrm->r;
    new->len += 1;
    #endif
  return (new->len);
}
