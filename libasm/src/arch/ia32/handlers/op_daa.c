/*
** $Id: op_daa.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_daa" opcode="0x27"/>
*/

int     op_daa(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_DAA;
  #if LIBASM_USE_OPERAND_VECTOR
  #endif
  return (new->len);
}
