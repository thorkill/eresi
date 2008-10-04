/*
** $Id: op_hlt.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_hlt" opcode="0xf4"/>
*/

int op_hlt(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_HLT;
  new->type = ASM_TYPE_STOP;
  return (new->len);
}
