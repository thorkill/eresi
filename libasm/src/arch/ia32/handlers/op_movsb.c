/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_movsb" opcode="0xa4"/>
 */

int op_movsb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->instr = ASM_MOVSB;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_STORE | ASM_TYPE_ASSIGN;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_YDEST, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_YDEST, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_XSRC, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_XSRC, new);
#endif

  return (new->len);
}
