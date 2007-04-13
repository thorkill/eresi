/*
** $Id: op_nop.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction name="nop" func="op_nop" opcode="0x90"/>
*/

int op_nop(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_NOP;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
