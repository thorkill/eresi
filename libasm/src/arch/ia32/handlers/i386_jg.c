/*
** $Id: i386_jg.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_jg" opcode="0x8f"/>
*/


int i386_jg(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->instr = ASM_BRANCH_S_GREATER;
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
