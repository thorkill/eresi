/*
** $Id: op_leave.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_leave" opcode="0xc9"/>
*/


int op_leave(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  //new->type = ASM_TYPE_RETPROC;
  new->instr = ASM_LEAVE;
  return (new->len);
}
