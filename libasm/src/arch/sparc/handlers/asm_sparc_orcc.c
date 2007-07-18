/*
**
** $Id: asm_sparc_orcc.c,v 1.9 2007-07-18 15:47:10 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_orcc(asm_instr * ins, u_char * buf, u_int len,
		     asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  ins->flagswritten = ASM_SP_FLAG_C | ASM_SP_FLAG_V | ASM_SP_FLAG_Z | ASM_SP_FLAG_N;

  ins->nb_op = 3;
  ins->op1.baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op3.baser = opcode.rs1;
  asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_REGISTER, ins);

  if (opcode.i == 0) {
    ins->op2.baser = opcode.rs2;
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
  }
  else {
    ins->op2.imm = opcode.imm;
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMMEDIATE, ins);
  }

  if (ins->op1.baser == ASM_REG_G0 &&
      ins->op2.content == ASM_SP_OTYPE_REGISTER &&
      ins->op3.baser == ASM_REG_G0) {

    ins->instr = ASM_SP_TST;
    ins->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
    ins->nb_op = 1;
    ins->op1 = ins->op2;
  }

  return 4;
}
