/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jmp_ap" opcode="0xea"/>
 */

int     op_jmp_ap(asm_instr *new, u_char *opcode, u_int len,
                  asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->instr = ASM_LBRANCH;
  new->len += 1;
  new->type = ASM_TYPE_IMPBRANCH;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ADDRESS, new);

  return (new->len);
}
