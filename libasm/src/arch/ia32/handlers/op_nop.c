/*
** $Id: op_nop.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
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
    return (new->len);
}
