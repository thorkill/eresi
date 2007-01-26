/*
** $Id: op_mov_rmv_iv.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_rmv_iv" opcode="0xc7"/>
*/

int op_mov_rmv_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_MOV;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rmv_iv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
