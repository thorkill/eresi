/*
** $Id: op_ret_i2.c,v 1.3 2007-05-11 16:40:58 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_ret_i2" opcode="0xc2"/>
*/

int op_ret_i2(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 2;
  new->type = ASM_TYPE_RETPROC | ASM_TYPE_TOUCHSP;
  new->spdiff = 4;
  new->instr = ASM_RET;
  new->ptr_instr = opcode;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_IMMEDIATEWORD, proc);
#else
    new->op1.type = ASM_OTYPE_IMMEDIATE;
    new->op1.content = ASM_OP_VALUE;
    new->op1.ptr = opcode + 1;
    new->op1.len = 2;
    new->op1.imm = 0;
    memcpy(&new->op1.imm, opcode + 1, 2);
    new->len += 2;
#endif
  return (new->len);
}
