/*
**
** $Id: asm_sparc_sth.c,v 1.5 2007-06-27 11:25:12 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_sth(asm_instr * ins, u_char * buf, u_int len,
	      asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  ins->type = ASM_TYPE_STORE;

  ins->nb_op = 2;
  if (opcode.i) {
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
    ins->op1.imm = opcode.imm;
  }
  else {
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
    ins->op1.indexr = opcode.rs2;
  }
  ins->op1.baser = opcode.rs1;

  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op2.baser = opcode.rd;

  if (ins->instr == ASM_SP_STB && ins->op2.baser == ASM_REG_G0) {
    ins->instr = ASM_SP_CLRB;
    ins->nb_op = 1;
  }
  else if (ins->instr == ASM_SP_STH && ins->op2.baser == ASM_REG_G0) {
    ins->instr = ASM_SP_CLRH;
    ins->nb_op = 1;
  }
  else if (ins->instr == ASM_SP_STW && ins->op2.baser == ASM_REG_G0) {
    ins->instr = ASM_SP_CLR;
    ins->nb_op = 1;
  }
  else if (ins->instr == ASM_SP_STX && ins->op2.baser == ASM_REG_G0) {
    ins->instr = ASM_SP_CLRX;
    ins->nb_op = 1;
  }
  
  return 4;

}
