/*
** $Id: op_pop_rmv.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_pop_rmv" opcode="0x8f"/>
*/

int op_pop_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_POP;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
  return (new->len);
}
