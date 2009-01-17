/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_group12" opcode="0x71"/>
*/

int			i386_group12(asm_instr *new, u_char *opcode,
				     u_int len, asm_processor *proc)
{
  int			olen;
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  switch (modrm->r)
    {
    case 2:
      new->instr = ASM_PSRLW;

      break;

    case 4:
      new->instr = ASM_PSRAW;
      break;

    case 6:
      new->instr = ASM_PSLLW;
      break;

    default:
      new->instr = ASM_BAD;
      return (new->len = 0);
      break;
    }
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1,					ASM_CONTENT_PMMX, new, 0));
#else
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1,					ASM_CONTENT_PMMX, new));
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen,				ASM_CONTENT_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen,				ASM_CONTENT_IMMEDIATEBYTE, new);
#endif
#else
  new->op[0].content = ASM_CONTENT_PMMX;
  new->op[0].size = ASM_OSIZE_QWORD;
  new->op[1].content = ASM_CONTENT_IMMEDIATE;
  new->op[1].size = ASM_OSIZE_BYTE;

  operand_rmb_ib(new, opcode + 1, len - 1, proc);
  new->op[0].regset = ASM_REGSET_MM;
#endif

  return (new->len);
}
