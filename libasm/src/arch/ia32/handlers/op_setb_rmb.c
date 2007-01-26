/*
** $Id: op_setb_rmb.c,v 1.1 2007-01-26 17:48:31 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="op_setb_rmb" opcode="0x92"/>
*/

int op_setb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_SET_U_LESS;
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmb(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len + 1;
  return (new->len);
}
