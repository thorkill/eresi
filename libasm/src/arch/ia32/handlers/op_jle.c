/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jle" opcode="0x7e"/>
*/

int  op_jle(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_S_LESS_EQUAL;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_SHORTJUMP, new);

  return (new->len);
}
