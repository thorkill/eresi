/*
** $Id: op_sti.c,v 1.3 2007-05-29 00:40:28 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sti" opcode="0xfb"/>
*/

int op_sti(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_CONTROL;
  new->instr = ASM_STI;
  return (new->len);
}
