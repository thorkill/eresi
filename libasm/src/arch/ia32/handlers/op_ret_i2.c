/*
** $Id: op_ret_i2.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_ret_i2" opcode="0xc2"/>
*/

int op_ret_i2(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 3;
   new->type = ASM_TYPE_RETPROC;
    new->instr = ASM_RET;
  new->ptr_instr = opcode;

    new->op1.type = ASM_OTYPE_IMMEDIATE;
    new->op1.content = ASM_OP_VALUE;
    new->op1.ptr = opcode + 1;
    new->op1.len = 2;
    new->op1.imm = 0;
    memcpy(&new->op1.imm, opcode + 1, 2);
  
  return (new->len);
}
