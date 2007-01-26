/*
** $Id: op_cmp_xchg.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="op_cmp_xchg" opcode="0xb1"/>
*/

int     op_cmp_xchg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_CMPXCHG;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
