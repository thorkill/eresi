/*
**
** $Id: asm_sparc_tcc.c,v 1.5 2007-06-16 20:24:26 strauss Exp $
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
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
    ins->op1.imm = opcode4.sw_trap;
  }
  else {
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
    ins->op1.index_reg = opcode4.rs2;
  }
  ins->op1.base_reg = opcode4.rs1;

  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_CC, ins);
  ins->op2.base_reg = (opcode4.cc & 0x3) + 4;
  
  return 4;

}
