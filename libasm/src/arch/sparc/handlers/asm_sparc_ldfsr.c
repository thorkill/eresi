/*
**
** $Id: asm_sparc_ldfsr.c,v 1.4 2007-04-22 20:48:41 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_ldfsr(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);
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
  ins->op1.type = ASM_SP_OTYPE_FREGISTER;
  ins->op1.base_reg = ASM_FREG_FSR;
      
  if (opcode.i) {
    ins->op2.type = ASM_SP_OTYPE_IMM_ADDRESS;
    ins->op2.imm = opcode.imm;
  }
  else {
   ins->op2.type = ASM_SP_OTYPE_REG_ADDRESS;
   ins->op2.index_reg = opcode.rs2;
  }
  ins->op2.base_reg = opcode.rs1;
  	  
  return 4;
}
