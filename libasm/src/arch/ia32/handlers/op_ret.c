/*
** $Id: op_ret.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
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
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
