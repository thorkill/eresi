/*
**
** $Id: asm_sparc_wr.c,v 1.6 2007-06-16 20:24:26 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_wr(asm_instr * ins, u_char * buf, u_int len,
	     asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_ASSIGN;

  if (opcode.rd == 1)
    ins->instr = ASM_SP_BAD;
  else if (opcode.rd == 15) {	/* SIR */
    ins->instr = ASM_SP_SIR;
    ins->type = ASM_TYPE_INT;
    ins->nb_op = 1;
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_IMMEDIATE, ins);
    ins->op1.imm = opcode.imm;
  }
  else { /* WR */
    ins->nb_op = 3;

    if (opcode.rd == 2) { /* WRCCR overwrites the condition codes */
      ins->type |= ASM_TYPE_FLAG;
      ins->flags = ASM_SP_FLAG_C | ASM_SP_FLAG_V | ASM_SP_FLAG_Z | ASM_SP_FLAG_N;
    }

    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_SREGISTER, ins);
    if (opcode.rd == 4 || opcode.rd == 5)	/* can't write PC or TICK */
      ins->op1.base_reg = ASM_SREG_BAD;
    else
      ins->op1.base_reg = opcode.rd;

    asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_REGISTER, ins);
    ins->op3.base_reg = opcode.rs1;

    if (opcode.i == 0) {
      asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
      ins->op2.base_reg = opcode.rs2;
    }
    else {
      asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMMEDIATE, ins);
      ins->op2.imm = opcode.imm;
    }
    
    if (ins->op1.base_reg == ASM_SREG_Y && ins->op3.base_reg == ASM_REG_G0) {
  	  	ins->instr = ASM_SP_MOV;
  	  	ins->nb_op = 2;
	}  
  }
  
  return 4;

}
