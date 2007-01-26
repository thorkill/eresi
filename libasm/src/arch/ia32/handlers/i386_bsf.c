/*
** $Id: i386_bsf.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 <i386 func="i386_bsf" opcode"0xbc"/>
*/

int i386_bsf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_BSF;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rm2(new, opcode + 1, len - 1, proc);
  return (new->len);
}
