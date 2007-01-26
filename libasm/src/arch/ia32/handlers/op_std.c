/*
** $Id: op_std.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_std" opcode="0xfd"/>
*/

int op_std(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_STD;
  return (new->len);
}
