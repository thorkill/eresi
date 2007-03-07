/*
**
** $Id: asm_sparc_std.c,v 1.3 2007-03-07 16:45:35 thor Exp $
**
*/
#include "libasm.h"

int
asm_sparc_std(asm_instr * ins, u_char * buf, u_int len,
	      asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  ins->type = ASM_TYPE_STORE;

  ins->nb_op = 2;
  if (opcode.i) {
    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
    ins->op1.imm = opcode.imm;
  }
  else {
    ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
    ins->op1.index_reg = opcode.rs2;
  }
  ins->op1.base_reg = opcode.rs1;

  ins->op2.type = ASM_SP_OTYPE_REGISTER;
  ins->op2.base_reg = opcode.rd;

  if (ins->instr == ASM_SP_STB && ins->op2.base_reg == ASM_REG_G0) {
    ins->instr = ASM_SP_CLRB;
    ins->nb_op = 1;
  }
  else if (ins->instr == ASM_SP_STH && ins->op2.base_reg == ASM_REG_G0) {
    ins->instr = ASM_SP_CLRH;
    ins->nb_op = 1;
  }
  else if (ins->instr == ASM_SP_STW && ins->op2.base_reg == ASM_REG_G0) {
    ins->instr = ASM_SP_CLR;
    ins->nb_op = 1;
  }
  else if (ins->instr == ASM_SP_STX && ins->op2.base_reg == ASM_REG_G0) {
    ins->instr = ASM_SP_CLRX;
    ins->nb_op = 1;
  }
  
  return 4;

}
