/*
**
** $Id: asm_sparc_casa.c,v 1.5 2007-06-16 20:24:25 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_casa(asm_instr * ins, u_char * buf, u_int len,
	       asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  ins->type = ASM_TYPE_LOAD | ASM_TYPE_ASSIGN | ASM_TYPE_TEST | ASM_TYPE_STORE;

  ins->nb_op = 3;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op1.base_reg = opcode.rd;
  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op2.base_reg = opcode.rs2;
  if (opcode.i) {
    asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
    ins->op3.imm = 0;
    ins->op3.address_space = -1;
  }
  else {
    asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
    ins->op3.index_reg = -1;
    ins->op3.address_space = opcode.none;
  }
  ins->op3.base_reg = opcode.rs1;

  /* Synthethics */
  if (ins->instr == ASM_SP_CASA
      && ins->op3.type == ASM_SP_OTYPE_REG_ADDRESS) {
    if (ins->op3.address_space == ASM_SP_ASI_P)
      ins->instr = ASM_SP_CAS;
    else if (ins->op3.address_space == ASM_SP_ASI_P_L) {
      ins->instr = ASM_SP_CASL;
      ins->op3.address_space = ASM_SP_ASI_P;
    }
  }
  else if (ins->instr == ASM_SP_CASXA &&
	   ins->op3.type == ASM_SP_OTYPE_REG_ADDRESS) {
    if (ins->op3.address_space == ASM_SP_ASI_P)
      ins->instr = ASM_SP_CASX;
    else if (ins->op3.address_space == ASM_SP_ASI_P_L) {
      ins->instr = ASM_SP_CASXL;
      ins->op3.address_space = ASM_SP_ASI_P;
    }
  }
  
  return 4;

}
