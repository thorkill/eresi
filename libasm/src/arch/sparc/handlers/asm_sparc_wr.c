/*
**
** $Id: asm_sparc_wr.c,v 1.10 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_wr(asm_instr * ins, unsigned char * buf, unsigned int len,
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
    ins->op[0].imm = opcode.imm;
    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_IMMEDIATE, ins);
  }
  else { /* WR */
    ins->nb_op = 3;

    if (opcode.rd == 2) { /* WRCCR overwrites the condition codes */
      ins->type |= ASM_TYPE_WRITEFLAG;
      ins->flagswritten = ASM_SP_FLAG_C | ASM_SP_FLAG_V | ASM_SP_FLAG_Z | ASM_SP_FLAG_N;
    }

    if (opcode.rd == 4 || opcode.rd == 5) /* can't write PC or TICK */
      ins->op[0].baser = ASM_SREG_BAD;
    else
      ins->op[0].baser = opcode.rd;

    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_SREGISTER, ins);

    ins->op[2].baser = opcode.rs1;
    asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_REGISTER, ins);

    if (opcode.i == 0) {
      ins->op[1].baser = opcode.rs2;
      asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_REGISTER, ins);
    }
    else {
      ins->op[1].imm = opcode.imm;
      asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_IMMEDIATE, ins);
    }

    if (ins->op[0].baser == ASM_SREG_Y && ins->op[2].baser == ASM_REG_G0) {
      ins->instr = ASM_SP_MOV;
      ins->nb_op = 2;
    }
  }

  return 4;
}
