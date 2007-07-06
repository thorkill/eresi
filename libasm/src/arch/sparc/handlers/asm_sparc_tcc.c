/*
**
** $Id: asm_sparc_tcc.c,v 1.7 2007-07-06 21:18:08 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_tcc(asm_instr * ins, u_char * buf, u_int len,
	      asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_decode_format4 opcode4;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  sparc_convert_format4(&opcode4, buf);

  inter = proc->internals;
  ins->type = ASM_TYPE_INT;

  ins->instr = inter->tcc_table[opcode4.cond];
  ins->nb_op = 2;
  if (opcode4.i) {
    ins->op1.baser = opcode4.rs1;
    ins->op1.imm = opcode4.sw_trap;
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
  }
  else {
    ins->op1.baser = opcode4.rs1;
    ins->op1.indexr = opcode4.rs2;
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
  }

  ins->op2.baser = (opcode4.cc & 0x3) + 4;
  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_CC, ins);

  return 4;
}
