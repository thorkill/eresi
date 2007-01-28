#include "libasm.h"

int
asm_sparc_addccc(asm_instr * ins, u_char * buf, u_int len,
		     asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];


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

  /* Synthethics */
  if (ins->instr == ASM_SP_SUBCC) {
    if (ins->op1.base_reg == ASM_REG_G0) {
      ins->instr = ASM_SP_CMP;
      ins->nb_op = 2;
      ins->op1 = ins->op2;
      ins->op2 = ins->op3;
    }
    else if ((ins->op1.base_reg == ins->op3.base_reg) &&
	     ins->op2.type == ASM_OTYPE_IMMEDIATE) {

      ins->instr = ASM_SP_DECCC;

      if (ins->op2.imm == 1)
	ins->nb_op = 1;
      else
	ins->nb_op = 2;
    }
  }
  else if (ins->instr == ASM_SP_SUB) {
    if ((ins->op1.base_reg == ins->op3.base_reg) &&
	ins->op2.type == ASM_OTYPE_IMMEDIATE) {

      ins->instr = ASM_SP_DEC;

      if (ins->op2.imm == 1)
	ins->nb_op = 1;
      else
	ins->nb_op = 2;
    }
    else if (ins->op3.base_reg == ASM_REG_G0 &&
	     ins->op2.type == ASM_OTYPE_REGISTER) {

      ins->instr = ASM_SP_NEG;
      if (ins->op2.base_reg == ins->op1.base_reg)
	ins->nb_op = 1;
      else
	ins->nb_op = 2;

    }
  }
  else if (ins->instr == ASM_SP_ADD) {
    if ((ins->op1.base_reg == ins->op3.base_reg) &&
	ins->op2.type == ASM_OTYPE_IMMEDIATE) {

      ins->instr = ASM_SP_INC;

      if (ins->op2.imm == 1)
	ins->nb_op = 1;
      else
	ins->nb_op = 2;
    }
  }
  else if (ins->instr == ASM_SP_ADDCC) {
    if ((ins->op1.base_reg == ins->op3.base_reg) &&
	ins->op2.type == ASM_OTYPE_IMMEDIATE) {

      ins->instr = ASM_SP_INCCC;

      if (ins->op2.imm == 1)
	ins->nb_op = 1;
      else
	ins->nb_op = 2;
    }
  }
  else if (ins->instr == ASM_SP_ORCC &&
	   ins->op1.base_reg == ASM_REG_G0 &&
	   ins->op2.type == ASM_SP_OTYPE_REGISTER &&
	   ins->op3.base_reg == ASM_REG_G0) {

    ins->instr = ASM_SP_TST;
    ins->nb_op = 1;
    ins->op1 = ins->op2;
  }
  else if ((ins->instr == ASM_SP_RESTORE ||
	    ins->instr == ASM_SP_SAVE) &&
	   ins->op2.type == ASM_SP_OTYPE_REGISTER &&
	   ins->op1.base_reg == ins->op2.base_reg &&
	   ins->op2.base_reg == ins->op3.base_reg) {

    ins->nb_op = 0;
  }
  else if (ins->instr == ASM_SP_XNOR &&
	   ins->op2.type == ASM_SP_OTYPE_REGISTER &&
	   ins->op2.base_reg == ASM_REG_G0) {

    ins->instr = ASM_SP_NOT;
    if (ins->op3.base_reg == ins->op1.base_reg)
      ins->nb_op = 1;
    else {
      ins->nb_op = 2;
      ins->op2 = ins->op3;
    }
  }
  else if (ins->instr == ASM_SP_ANDCC && ins->op1.base_reg == ASM_REG_G0) {

    ins->instr = ASM_SP_BTST;
    ins->nb_op = 2;
    ins->op1 = ins->op3;
  }
  else if (ins->instr == ASM_SP_ANDN &&
	   ins->op1.base_reg == ins->op3.base_reg) {

    ins->instr = ASM_SP_BCLR;
    ins->nb_op = 2;
  }
  else if (ins->instr == ASM_SP_XOR
	   && ins->op1.base_reg == ins->op3.base_reg) {

    ins->instr = ASM_SP_BTOG;
    ins->nb_op = 2;
  }
  else if (ins->instr == ASM_SP_OR) {
    if (ins->op1.base_reg == ins->op3.base_reg) {
      ins->instr = ASM_SP_BSET;
      ins->nb_op = 2;
    }
    else if (ins->op3.base_reg == ASM_REG_G0) {
      if (ins->op2.type == ASM_OTYPE_REGISTER &&
	  ins->op2.base_reg == ASM_REG_G0) {

	ins->instr = ASM_SP_CLR;
	ins->nb_op = 1;
      }
      else {
	ins->instr = ASM_SP_MOV;
	ins->nb_op = 2;
      }
    }
  }
  else if (ins->instr == ASM_SP_RD && ins->op1.base_reg == ASM_SREG_Y) {

    ins->instr = ASM_SP_MOV;
  }
  else if (ins->instr == ASM_SP_WR &&
	   ins->op1.base_reg == ASM_SREG_Y
	   && ins->op3.base_reg == ASM_REG_G0) {

    ins->instr = ASM_SP_MOV;
    ins->nb_op = 2;
  }


}
