/*
** $Id: i386_group14.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_group14" opcode="0x73"/>
 */

int     i386_group14(asm_instr *new, unsigned char *opcode, unsigned int len,
		     asm_processor *proc)
{
  int	olen;
  struct s_modrm *modrm = (struct s_modrm *) opcode + 1;

  new->ptr_instr = opcode;
  new->len += 1;
  switch (modrm->r)
    {
    case 2:
      new->instr = ASM_PSRLQ;
      break;

    case 6:
      new->instr = ASM_PSLLQ;
      new->op[0].type = ASM_OTYPE_PMMX;
      new->op[0].size = ASM_OSIZE_QWORD;
      new->op[1].type = ASM_OTYPE_IMMEDIATE;
      new->op[1].size = ASM_OSIZE_BYTE;

      operand_rmb_ib(new, opcode + 1, len - 1, proc);
      new->op[0].regset = ASM_REGSET_MM;
      break;

    default:
      new->instr = ASM_BAD;
      return (new->len = 0);
      break;
    }

#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_PMMX,					new, 0));
#else
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_PMMX,					new));
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen,				ASM_OTYPE_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen,				ASM_OTYPE_IMMEDIATEBYTE, new);
#endif
#else
  new->op[0].type = ASM_OTYPE_PMMX;
  new->op[0].size = ASM_OSIZE_QWORD;
  new->op[1].type = ASM_OTYPE_IMMEDIATE;
  new->op[1].size = ASM_OSIZE_BYTE;

  operand_rmb_ib(new, opcode + 1, len - 1, proc);
  new->op[0].regset = ASM_REGSET_MM;
#endif

  return (new->len);
}
