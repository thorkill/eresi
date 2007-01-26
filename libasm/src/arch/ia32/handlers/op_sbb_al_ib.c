/*
** $Id: op_sbb_al_ib.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sbb_al_ib" opcode="0x1c"/>
*/

int op_sbb_al_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  
  new->instr = ASM_SBB;
  new->len += 2;
  new->ptr_instr = opcode;
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op1.size = new->op2.size = ASM_OSIZE_BYTE;
  
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_AL;
  new->op1.regset = ASM_REGSET_R8;
  
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = 1;
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, 1);
  return (new->len);
}
