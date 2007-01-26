/*
** $Id: op_setnp_rmb.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="op_setnp_rmb" opcode="0x9b"/>
*/

int op_setnp_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_SET_NOT_PARITY;
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmb(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len + 1;
  return (new->len);
}
