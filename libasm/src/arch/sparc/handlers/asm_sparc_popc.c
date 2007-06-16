/*
**
** $Id: asm_sparc_popc.c,v 1.5 2007-06-16 20:24:25 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_popc(asm_instr * ins, u_char * buf, u_int len,
	       asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_ASSIGN;

  if (opcode.rs1 == 0) {
    ins->instr = ASM_SP_POPC;
    ins->nb_op = 2;

    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);
    ins->op1.base_reg = opcode.rd;

    if (opcode.i == 0) {
      asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
      ins->op2.base_reg = opcode.rs2;
    }
    else {
      asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMMEDIATE, ins);
      ins->op2.imm = opcode.imm;
    }
  }
  else
    ins->instr = ASM_SP_BAD;
    
  return 4;

}
