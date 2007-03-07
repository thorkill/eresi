/*
**
** $Id: asm_sparc_xnor.c,v 1.3 2007-03-07 16:45:35 thor Exp $
**
*/
#include "libasm.h"

int
asm_sparc_xnor(asm_instr * ins, u_char * buf, u_int len,
		     asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_ARITH;

  ins->nb_op = 3;
  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  ins->op1.base_reg = opcode.rd;
  ins->op3.type = ASM_SP_OTYPE_REGISTER;
  ins->op3.base_reg = opcode.rs1;

  if (opcode.i == 0) {
    ins->op2.type = ASM_SP_OTYPE_REGISTER;
    ins->op2.base_reg = opcode.rs2;
  }
  else {
    ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
    ins->op2.imm = opcode.imm;
  }

  if (ins->op2.type == ASM_SP_OTYPE_REGISTER &&
	   ins->op2.base_reg == ASM_REG_G0) {

    ins->instr = ASM_SP_NOT;
    if (ins->op3.base_reg == ins->op1.base_reg)
      ins->nb_op = 1;
    else {
      ins->nb_op = 2;
      ins->op2 = ins->op3;
    }
  }
  
  return 4;

}
