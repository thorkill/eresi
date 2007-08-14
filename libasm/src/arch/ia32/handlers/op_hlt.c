/*
** $Id: op_hlt.c,v 1.4 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_hlt" opcode="0xf4"/>
*/

int op_hlt(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_HLT;
  new->type = ASM_TYPE_STOP;
  return (new->len);
}
