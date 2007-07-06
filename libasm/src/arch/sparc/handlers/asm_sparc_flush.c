/*
**
** $Id: asm_sparc_flush.c,v 1.6 2007-07-06 21:18:08 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_flush(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_OTHER;

  ins->nb_op = 1;
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
  
  return 4;
}
