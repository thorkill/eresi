/*
** $Id: i386_jbe.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_jbe" opcode="0x86"/>
*/


int i386_jbe(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  // new->type = IS_COND_BRANCH;
  new->instr = ASM_BRANCH_U_LESS_EQUAL;
  new->len += 1;

#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_JUMP, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_JUMP, new);
#endif
#else

  new->op[0].type = ASM_OTYPE_JUMP;
  new->op[0].content = ASM_OP_VALUE | ASM_OP_ADDRESS;
  new->op[0].ptr = opcode + 1;
  new->op[0].len = 4;
  memcpy(&new->op[0].imm, opcode + 1, 4);
  new->len += 4;
#endif
  return (new->len);
}
