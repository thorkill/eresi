/*
**
** $Id: asm_sparc_subcc.c,v 1.6 2007-06-16 20:24:26 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_subcc(asm_instr * ins, u_char * buf, u_int len,
		     asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_ARITH | ASM_TYPE_FLAG;
  ins->flags = ASM_SP_FLAG_C | ASM_SP_FLAG_V | ASM_SP_FLAG_Z | ASM_SP_FLAG_N;

  ins->nb_op = 3;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op1.base_reg = opcode.rd;
  asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op3.base_reg = opcode.rs1;

  if (opcode.i == 0) {
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
    ins->op2.base_reg = opcode.rs2;
  }
  else {
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMMEDIATE, ins);
    ins->op2.imm = opcode.imm;
  }

  if (ins->op1.base_reg == ASM_REG_G0) {
    ins->instr = ASM_SP_CMP;
    ins->type |= ASM_TYPE_TEST;
    ins->nb_op = 2;
    ins->op1 = ins->op2;
    ins->op2 = ins->op3;
  }
  else if ((ins->op1.base_reg == ins->op3.base_reg) &&
	ins->op2.type == ASM_OTYPE_IMMEDIATE) {

    ins->instr = ASM_SP_DECCC;

    if (ins->op2.imm == 1)
	  ins->nb_op = 1;
    else
	  ins->nb_op = 2;
  }
  
  return 4;

}
