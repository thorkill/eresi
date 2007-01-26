/*
** $Id: op_ud2a.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int     op_ud2a(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_UD2A;
  return (new->len);
}
