/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_group14" opcode="0x73"/>
 */

int     i386_group14(asm_instr *new, u_char *opcode, u_int len,
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
      break;

    default:
      new->instr = ASM_BAD;
      return (new->len = 0);
      break;
    }

#if LIBASM_USE_OPERAND_VECTOR
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_PMMX, new));
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen, ASM_CONTENT_IMMEDIATEBYTE, new);
#else
  new->op[0].content = ASM_CONTENT_PMMX;
  new->op[0].size = ASM_OSIZE_QWORD;
  new->len += (olen = operand_rmv(&new->op[0], opcode + 1, ins->proc));
  new->op[0].regset = ASM_REGSET_MM;
  new->op[0].sbaser = get_reg_intel(new->op[0].baser, new->op[0].regset);

  new->op[1].content = ASM_CONTENT_IMMEDIATE;
  new->op[1].type = ASM_OPTYPE_IMM;
  new->op[1].size = ASM_OSIZE_BYTE;
  new->op[1].ptr = opcode + olen + 1;
  new->op[1].imm = 0;
  new->op[1].len = 1;
  if (*(opcode + olen + 1)>= 0x80)
  {
    len = asm_proc_opsize(ins->proc) ? 2 : 4;
    memset(&new->op[1].imm, 0xff, len);
  }
  memcpy(&new->op[1].imm, opcode + olen + 1, 1);

  new->len += 1;
#endif

  return (new->len);
}
