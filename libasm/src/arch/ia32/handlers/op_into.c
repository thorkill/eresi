/*
** $Id: op_into.c,v 1.4 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction opcode="0xce" func="op_into"/>
*/

int op_into(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_INTO;
  new->type = ASM_TYPE_INT | ASM_TYPE_TOUCHSP | ASM_TYPE_READFLAG;
  new->flagsread = ASM_FLAG_OF;
  return (new->len);
}
