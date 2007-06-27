/*
**
** $Id: asm_sparc_sll.c,v 1.6 2007-06-27 11:25:12 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_sll(asm_instr * ins, u_char * buf, u_int len,
	      asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->type = ASM_TYPE_ARITH;

  ins->instr = inter->shift_table[opcode.op3 - 0x25 +
				  ((opcode.imm & 0x1000) ? 1 : 0)];

  ins->nb_op = 3;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op1.baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op3.baser = opcode.rs1;

  if (opcode.i == 0) {
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
    ins->op2.baser = opcode.rs2;
  }
  else {
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMMEDIATE, ins);
    ins->op2.imm = opcode.shcnt;
  }

  /* Synthetics */
  if (ins->instr == ASM_SP_SRL &&
      ins->op2.type == ASM_SP_OTYPE_REGISTER &&
      ins->op2.baser == ASM_REG_G0) {

    ins->instr = ASM_SP_CLRUW;
    if (ins->op3.baser == ins->op1.baser)
      ins->nb_op = 1;
    else {
      ins->nb_op = 2;
      ins->op2 = ins->op3;
    }
  }
  else if (ins->instr == ASM_SP_SRA &&
	   ins->op2.type == ASM_SP_OTYPE_REGISTER &&
	   ins->op2.baser == ASM_REG_G0) {

    ins->instr = ASM_SP_SIGNX;
    if (ins->op3.baser == ins->op1.baser)
      ins->nb_op = 1;
    else {
      ins->nb_op = 2;
      ins->op2 = ins->op3;
    }
  }
  
  return 4;

}
