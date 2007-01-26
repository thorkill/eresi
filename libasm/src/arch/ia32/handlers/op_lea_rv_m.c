/*
** $Id: op_lea_rv_m.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_lea_rv_m" opcode="0x8d"/>
*/

int op_lea_rv_m(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_LEA;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_MEMORY;
  
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
