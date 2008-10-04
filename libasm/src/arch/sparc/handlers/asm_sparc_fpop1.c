/*
**
** $Id: asm_sparc_fpop1.c,v 1.8 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_fpop1(asm_instr * ins, unsigned char * buf, unsigned int len,
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
  else { /* 0x40 < opf < 0x69 - add, sub, mul, div */
    ins->nb_op = 3;
    ins->op[2].baser = opcode.rs1;
    asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_FREGISTER, ins);
  }

  ins->op[0].baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op[1].baser = opcode.rs2;
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_FREGISTER, ins);

  return 4;

}
