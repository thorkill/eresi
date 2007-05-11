/*
** $Id: op_pusha.c,v 1.3 2007-05-11 16:40:58 heroine Exp $
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
    new->type = ASM_TYPE_TOUCHSP;
    new->spdiff = -4 * 8;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
