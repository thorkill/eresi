/*
** $Id: op_into.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction opcode="0xce" func="op_into"/>
*/

int op_into(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->len += 1;
    new->instr = ASM_INTO;
  return (new->len);
}
