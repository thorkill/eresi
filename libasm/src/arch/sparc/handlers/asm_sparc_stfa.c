/*
**
** $Id: asm_sparc_stfa.c,v 1.4 2007-06-16 20:24:26 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_stfa(asm_instr * ins, u_char * buf, u_int len,
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
    ins->op1.address_space = -1;
  }
  else {
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
    ins->op1.index_reg = opcode.rs2;
    ins->op1.address_space = opcode.none;
  }
  ins->op1.base_reg = opcode.rs1;

  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op2.base_reg = opcode.rd;
  
  return 4;

}
