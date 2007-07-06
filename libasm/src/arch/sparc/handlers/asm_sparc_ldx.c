/*
**
** $Id: asm_sparc_ldx.c,v 1.7 2007-07-06 21:18:08 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_ldx(asm_instr * ins, u_char * buf, u_int len,
	      asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  ins->type = ASM_TYPE_LOAD | ASM_TYPE_ASSIGN;
  
  ins->nb_op = 2;
  ins->op1.baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);

  if (opcode.i) {
    ins->op2.baser = opcode.rs1;
    ins->op2.imm = opcode.imm;
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
  }
  else {
    ins->op2.baser = opcode.rs1;
    ins->op2.indexr = opcode.rs2;
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
  }

  return 4;
}
