/*
** $Id: op_int1.c,v 1.4 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_lock" opcode="0xf1"/>
*/

int op_int1(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_INT1;
  new->type = ASM_TYPE_INT | ASM_TYPE_TOUCHSP;
  return (new->len);
}
