/*
** $Id: op_seta_rmb.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="op_seta_rmb" opcode="0x97"/>
*/

int op_seta_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_SET_U_GREATER;
    new->op1.type = ASM_OTYPE_ENCODED;
    operand_rmb(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len + 1;
  return (new->len);
}
