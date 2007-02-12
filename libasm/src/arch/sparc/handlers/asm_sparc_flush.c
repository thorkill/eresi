#include "libasm.h"

int
asm_sparc_flush(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_OTHER;

  ins->nb_op = 1;
  if (opcode.i) {
    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
    ins->op1.imm = opcode.imm;
  }
  else {
    ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
    ins->op1.index_reg = opcode.rs2;
  }
  ins->op1.base_reg = opcode.rs1;
  
  return 4;
}
