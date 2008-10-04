/*
** $Id: i386_group15.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="i386_group15" opcode="0xae"/>
 */

int i386_group15(asm_instr *new, unsigned char *opcode, unsigned int len,
		 asm_processor *proc)
{
  struct s_modrm        *modrm;
  modrm = (struct s_modrm *) opcode + 1;

  new->len += 1;

  switch(modrm->r)
    {
    case 2:
      new->instr = ASM_LDMXCSR;
      new->op[0].type = ASM_OTYPE_ENCODED;
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new);
#endif
#else
      operand_rmv(&new->op[0], opcode + 1, len - 1, proc);
      new->len += new->op[0].len;
#endif
      break;

    case 3:
      new->instr = ASM_STMXCSR;
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

    case 7:
      new->instr = ASM_CLFLUSH;
      new->op[0].type = ASM_OTYPE_GENERAL;
      break;

    default:
      new->len = 0;
      break;
    }

  return (new->len);
}
