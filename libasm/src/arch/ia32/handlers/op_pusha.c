/*
** $Id: op_pusha.c,v 1.5 2007-08-20 07:21:04 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_pusha" opcode="0x60"/>
*/

int op_pusha(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSHA;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE;
  new->spdiff = -4 * 8;
  return (new->len);
}
