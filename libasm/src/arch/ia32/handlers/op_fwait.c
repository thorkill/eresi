/*
** $Id: op_fwait.c,v 1.6 2007-10-14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_fwait" opcode="0x9b"/>
 */

int op_fwait(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_FWAIT;
  new->type = ASM_TYPE_NOP;
  return (new->len);
}
