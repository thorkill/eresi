/*
**
** $Id: asm_sparc_fpop1.c,v 1.5 2007-06-16 20:24:25 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_fpop1(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  
  ins->instr = inter->fpop1_table[opcode.opf];
  ins->type = ASM_TYPE_ARITH;
  if (opcode.opf < 0x40 || opcode.opf >= 0x70) {
  	ins->type = ASM_TYPE_ASSIGN;
    ins->nb_op = 2;
  }
  else {			/* 0x40 < opf < 0x69 - add, sub, mul, div */
    ins->nb_op = 3;
    asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_FREGISTER, ins);
    ins->op3.base_reg = opcode.rs1;
  }

  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op1.base_reg = opcode.rd;
  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op2.base_reg = opcode.rs2;
  
  return 4;

}
