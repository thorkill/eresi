/*
** $Id: op_mov_rb_rmb.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_rb_rmb" opcode="0x8a"/>
*/

int op_mov_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
    new->instr = ASM_MOV;
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rb_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}
