/*
** $Id: op_int_3.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_int_3" opcode="0xcc"/>
*/

int op_int_3(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->len += 1;
    new->instr = ASM_INT3;
    #if LIBASM_USE_OPERAND_VECTOR
    #endif
  return (new->len);
}
