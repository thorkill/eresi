/*
** $Id: i386_bsr_rv_rmb.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_bsr_rv_rmb" opcode="0xbd"/>
 */

int i386_bsr_rv_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->instr = ASM_BSR;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
    operand_rv_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}
