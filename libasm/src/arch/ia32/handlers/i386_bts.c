/*
** $Id: i386_bts.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_bts" opcode="0xab"/>
 */

int i386_bts(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->instr = ASM_BTS;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op2.type = ASM_OTYPE_GENERAL;
    operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
