/*
** $Id: op_pusha.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_pusha" opcode="0x60"/>
*/

int op_pusha(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->ptr_instr = opcode;
    new->instr = ASM_PUSHA;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
