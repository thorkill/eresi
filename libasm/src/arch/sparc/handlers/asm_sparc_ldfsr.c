/*
**
** $Id: asm_sparc_ldfsr.c,v 1.5 2007-06-16 20:24:25 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_ldfsr(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  ins->type = ASM_TYPE_LOAD | ASM_TYPE_ASSIGN;
  
  if (opcode.rd == 0)
    ins->instr = ASM_SP_LDFSR;
  else if (opcode.rd == 1)
    ins->instr = ASM_SP_LDXFSR;
  else {
   	ins->instr = ASM_SP_BAD;
   	return 4;
  }
  	  
  ins->nb_op = 2;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op1.base_reg = ASM_FREG_FSR;
      
  if (opcode.i) {
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
    ins->op2.imm = opcode.imm;
  }
  else {
   asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
   ins->op2.index_reg = opcode.rs2;
  }
  ins->op2.base_reg = opcode.rs1;
  	  
  return 4;
}
