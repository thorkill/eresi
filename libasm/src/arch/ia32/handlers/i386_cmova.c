/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_cmova" opcode="0x47"/>
 */

int     i386_cmova(asm_instr *new, u_char *opcode, u_int len,
		   asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_CMOVA;

  new->type = ASM_TYPE_ASSIGN | ASM_TYPE_COMPARISON;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERAL,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERAL,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODED,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODED,				new);
#endif

  return (new->len);
}
