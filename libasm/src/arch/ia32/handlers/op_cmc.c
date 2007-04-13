/*
** $Id: op_cmc.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmc" opcode="0xf5"/>
*/

int op_cmc(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_CMC;
  #if LIBASM_USE_OPERAND_VECTOR
  #endif
  return (new->len);
}
