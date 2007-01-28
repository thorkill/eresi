#include "libasm.h"

int
asm_sparc_popc(asm_instr * ins, u_char * buf, u_int len,
	       asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];


  if (opcode.rs1 == 0) {
    ins->instr = ASM_SP_POPC;
    ins->nb_op = 2;

    ins->op1.type = ASM_SP_OTYPE_REGISTER;
    ins->op1.base_reg = opcode.rd;

    if (opcode.i == 0) {
      ins->op2.type = ASM_SP_OTYPE_REGISTER;
      ins->op2.base_reg = opcode.rs2;
    }
    else {
      ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
      ins->op2.imm = opcode.imm;
    }
  }
  else
    ins->instr = ASM_SP_BAD;
    
  return 4;

}
