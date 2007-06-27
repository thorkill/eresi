/*
**
** $Id: asm_sparc_lduha.c,v 1.6 2007-06-27 11:25:12 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_lduha(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  ins->type = ASM_TYPE_LOAD | ASM_TYPE_ASSIGN;

  ins->nb_op = 2;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op1.baser = opcode.rd;

  if (opcode.i) {
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
    ins->op2.imm = opcode.imm;
    ins->op2.address_space = -1;
  }
  else {
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
    ins->op2.indexr = opcode.rs2;
    ins->op2.address_space = opcode.none;
  }
  ins->op2.baser = opcode.rs1;
  
  return 4;

}
