/*
** $Id: op_pusha.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
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
  return (new->len);
}
