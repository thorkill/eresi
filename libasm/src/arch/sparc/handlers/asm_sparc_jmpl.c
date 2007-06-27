/*
**
** $Id: asm_sparc_jmpl.c,v 1.6 2007-06-27 11:25:12 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_jmpl(asm_instr * ins, u_char * buf, u_int len,
	 asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_IMPBRANCH;

  ins->nb_op = 2;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op1.baser = opcode.rd;
  if (opcode.i) {
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
    ins->op2.imm = opcode.imm;
  }
  else {
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
    ins->op2.indexr = opcode.rs2;
  }
  ins->op2.baser = opcode.rs1;

  if (ins->op1.baser == ASM_REG_O7) {
    ins->nb_op = 1;
    ins->instr = ASM_SP_CALL;
    ins->type = ASM_TYPE_CALLPROC;
    ins->op1 = ins->op2;
  }
  else if (ins->op1.baser == ASM_REG_G0) {
    if (ins->op2.type == ASM_SP_OTYPE_IMM_ADDRESS &&
  ins->op2.baser == ASM_REG_I7 && ins->op2.imm == 8) {

      ins->instr = ASM_SP_RET;
      ins->type = ASM_TYPE_RETPROC;
      ins->nb_op = 0;
    }
    else if (ins->op2.type == ASM_SP_OTYPE_IMM_ADDRESS &&
       ins->op2.baser == ASM_REG_O7 && ins->op2.imm == 8) {

      ins->instr = ASM_SP_RETL;
      ins->type = ASM_TYPE_RETPROC;
      ins->nb_op = 0;
    }
    else {
      ins->instr = ASM_SP_JMP;
      ins->nb_op = 1;
      ins->op1 = ins->op2;
    }
  }
  
  return 4;

}
