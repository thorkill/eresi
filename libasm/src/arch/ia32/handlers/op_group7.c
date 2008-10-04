/*
** $Id: op_group7.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int     op_group7(asm_instr *new, unsigned char *opcode, unsigned int len,
		  asm_processor *proc)
{
  struct s_modrm        *modrm;

  new->len += 1;
  modrm = (struct s_modrm *) opcode + 1;

  switch(modrm->r)
    {
    case 0:
      new->instr = ASM_SGDT;
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new);
#endif
#else
      new->op[0].type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op[0], opcode + 1, len - 1, proc);
      new->len += new->op[0].len;
#endif
      break;
    case 1:
      new->instr = ASM_SIDT;
      break;
    case 2:
      new->instr = ASM_LGDT;
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new);
#endif
#else
      new->op[0].type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op[0], opcode + 1, len - 1, proc);
      new->len += new->op[0].len;
#endif
      break;
    case 3:
      new->instr = ASM_LIDT;
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new);
#endif
#else
      new->op[0].type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op[0], opcode + 1, len - 1, proc);
      new->len += new->op[0].len;
#endif
      break;
    case 4:
      break;
    case 5:
      new->instr = ASM_BAD;
      break;
    case 6:

      break;
    case 7:

      break;
    }
  return (new->len);
}
