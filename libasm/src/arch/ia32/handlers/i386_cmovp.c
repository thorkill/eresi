/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_cmovp" opcode="0x4a"/>
 */

int     i386_cmovp(asm_instr *new, u_char *opcode, u_int len,
		   asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_CMOVP;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERAL, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODED, new);
#else
  new->op[0].content = ASM_CONTENT_GENERAL;
  new->op[1].content = ASM_CONTENT_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
