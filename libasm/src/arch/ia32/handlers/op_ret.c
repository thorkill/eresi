/*
** $Id: op_ret.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_ret" opcode="0xc3"/>
*/


int op_ret(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_RETPROC;
  new->instr = ASM_RET;
  return (new->len);
}
