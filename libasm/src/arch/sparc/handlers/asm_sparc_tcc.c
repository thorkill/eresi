/*
**
** $Id: asm_sparc_tcc.c,v 1.4 2007-04-22 20:48:41 strauss Exp $
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
  sparc_convert_format3(&opcode, buf, proc);
  sparc_convert_format4(&opcode4, buf, proc);

  inter = proc->internals;
  ins->type = ASM_TYPE_INT;

  ins->instr = inter->tcc_table[opcode4.cond];
  ins->nb_op = 2;
  if (opcode4.i) {
    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
    ins->op1.imm = opcode4.sw_trap;
  }
  else {
    ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
    ins->op1.index_reg = opcode4.rs2;
  }
  ins->op1.base_reg = opcode4.rs1;

  ins->op2.type = ASM_SP_OTYPE_CC;
  ins->op2.base_reg = (opcode4.cc & 0x3) + 4;
  
  return 4;

}
