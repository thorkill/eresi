/*
** $Id: op_pushf.c,v 1.3 2007-05-11 16:40:58 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction name="pushf" func="op_pushf" opcode="0x9c"/>
*/


int op_pushf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_PUSHF;
    new->type = ASM_TYPE_TOUCHSP;
    new->spdiff = -4;

#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
