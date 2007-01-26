/*
** $Id: i386_imul_rv_rmv.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_imul_rv_rmv" opcode="0xaf"/>
*/

int i386_imul_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->instr = ASM_IMUL;
    new->len += 1;
    new->op1.type = ASM_OTYPE_GENERAL;
    new->op1.size = ASM_OSIZE_VECTOR;
    new->op2.type = ASM_OTYPE_ENCODED;
    new->op2.size = ASM_OSIZE_VECTOR;
    operand_rv_rmv(new, opcode + 1, len - 1, proc);
    return (new->len);
}
