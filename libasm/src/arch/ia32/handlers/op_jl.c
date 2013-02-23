/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jl" opcode="0x7c"/>
*/

int  op_jl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_S_LESS;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_SHORTJUMP, new);

  return (new->len);
}
