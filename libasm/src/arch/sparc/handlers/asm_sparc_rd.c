#include "libasm.h"

int
asm_sparc_rd(asm_instr * ins, u_char * buf, u_int len,
	     asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_LOAD;

  if (opcode.rs1 != 15) {	/* RD*(-PR) */
    ins->nb_op = 2;
    ins->op1.type = ASM_SP_OTYPE_REGISTER;
    ins->op1.base_reg = opcode.rd;
    ins->op2.type = ASM_SP_OTYPE_SREGISTER;
    ins->op2.base_reg = opcode.rs1;
    
    /* Synthetics */
    if (ins->op1.base_reg == ASM_SREG_Y) {
  	  	ins->instr = ASM_SP_MOV;
  	}
  }
  else {
    if (opcode.rd != 0)
      ins->instr = ASM_SP_BAD;
    else if (opcode.i == 0) {	/* STBAR */
      ins->type = ASM_TYPE_OTHER;
      ins->instr = ASM_SP_STBAR;
    }
    else if (opcode.i == 1) {	/* MEMBAR */
      ins->instr = ASM_SP_MEMBAR;
      ins->type = ASM_TYPE_OTHER;
      ins->nb_op = 1;
      ins->op1.type = ASM_SP_OTYPE_IMMEDIATE;
      /* operand = cmask OR mmask */
      ins->op1.imm = ((opcode.imm & 0x70) >> 4) | (opcode.imm & 0xf);
    }
  }
  
  return 4;
}
