/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_loopne" opcode="0xe0"/>
*/

int op_loopne(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->instr = ASM_LOOPNE;
  new->type = ASM_TYPE_CONDBRANCH | ASM_TYPE_READFLAG;
  new->ptr_instr = opcode;
  new->len += 1;
  new->flagsread = ASM_FLAG_ZF;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_SHORTJUMP, new);

  return (new->len);
}
