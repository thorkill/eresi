/*
**
** $Id: asm_sparc_wr.c,v 1.8 2007-07-06 21:18:08 strauss Exp $
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
    ins->op1.imm = opcode.imm;
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_IMMEDIATE, ins);
  }
  else { /* WR */
    ins->nb_op = 3;

    if (opcode.rd == 2) { /* WRCCR overwrites the condition codes */
      ins->type |= ASM_TYPE_FLAG;
      ins->flags = ASM_SP_FLAG_C | ASM_SP_FLAG_V | ASM_SP_FLAG_Z | ASM_SP_FLAG_N;
    }

    if (opcode.rd == 4 || opcode.rd == 5) /* can't write PC or TICK */
      ins->op1.baser = ASM_SREG_BAD;
    else
      ins->op1.baser = opcode.rd;

    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_SREGISTER, ins);

    ins->op3.baser = opcode.rs1;
    asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_REGISTER, ins);

    if (opcode.i == 0) {
      ins->op2.baser = opcode.rs2;
      asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
    }
    else {
      ins->op2.imm = opcode.imm;
      asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMMEDIATE, ins);
    }

    if (ins->op1.baser == ASM_SREG_Y && ins->op3.baser == ASM_REG_G0) {
      ins->instr = ASM_SP_MOV;
      ins->nb_op = 2;
    }
  }

  return 4;
}
