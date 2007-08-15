/*
** $Id: op_fwait.c,v 1.4 2007-08-15 21:30:20 strauss Exp $
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
  new->type = ASM_TYPE_OTHER;
  return (new->len);
}
