#include "libasm.h"

int
asm_sparc_fpop1(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];


  ins->instr = inter->fpop1_table[opcode.opf];
  if (opcode.opf < 0x40 || opcode.opf >= 0x70) {
    ins->nb_op = 2;
  }
  else {			/* 0x40 < opf < 0x69 - add, sub, mul, div */
    ins->nb_op = 3;
    ins->op3.type = ASM_SP_OTYPE_FREGISTER;
    ins->op3.base_reg = opcode.rs1;
  }

  ins->op1.type = ASM_SP_OTYPE_FREGISTER;
  ins->op1.base_reg = opcode.rd;
  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
  ins->op2.base_reg = opcode.rs2;
  
  return 4;

}
