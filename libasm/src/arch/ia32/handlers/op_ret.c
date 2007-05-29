/*
** $Id: op_ret.c,v 1.4 2007-05-29 00:40:28 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_ret" opcode="0xc3"/>
*/


int op_ret(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_RETPROC | ASM_TYPE_TOUCHSP;
  new->spdiff = 4;
  new->instr = ASM_RET;
  return (new->len);
}
