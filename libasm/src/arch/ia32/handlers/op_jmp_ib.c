/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jmp_ib" opcode="0xeb"/>
 */

int op_jmp_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->instr = ASM_BRANCH;

  new->type = ASM_TYPE_IMPBRANCH;
  new->ptr_instr = opcode;
  new->len += 1;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_SHORTJUMP, new);

  return (new->len);
}
