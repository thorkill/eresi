/*
** $Id: op_fwait.c,v 1.5 2007-08-20 07:21:04 strauss Exp $
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
