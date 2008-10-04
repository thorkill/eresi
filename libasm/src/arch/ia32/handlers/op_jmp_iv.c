/*
** $Id: op_jmp_iv.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jmp_iv" opcode="0xe9"/>
 */

int op_jmp_iv(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->instr = ASM_BRANCH;
  new->type = ASM_TYPE_IMPBRANCH;
  new->ptr_instr = opcode;
  new->len += 1;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_JUMP, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_JUMP, new);
#endif

  return (new->len);
}
