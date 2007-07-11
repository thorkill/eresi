/*
**
** $Id: asm_sparc_stqf.c,v 1.7 2007-07-11 22:06:47 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_stqf(asm_instr * ins, u_char * buf, u_int len,
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
    ins->op1.baser = opcode.rs1;
    ins->op1.imm = opcode.imm;
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
  }
  else {
    ins->op1.baser = opcode.rs1;
    ins->op1.indexr = opcode.rs2;
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
  }

  ins->op2.baser = ((opcode.rd & 1) << 5) | (opcode.rd & 0x1E);
  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_FREGISTER, ins);

  return 4;
}
