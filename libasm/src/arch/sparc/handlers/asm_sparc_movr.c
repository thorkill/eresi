/**
* @file libasm/src/arch/sparc/handlers/asm_sparc_movr.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_movr(asm_instr * ins, u_char * buf, u_int len,
	       asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->type = ASM_TYPE_ASSIGN;

  ins->instr = inter->movr_table[opcode.rcond];
  ins->nb_op = 3;
  ins->op[0].baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op[2].baser = opcode.rs1;
  asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_REGISTER, ins);

  if (opcode.i == 0) {
    ins->op[1].baser = opcode.rs2;
    asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_REGISTER, ins);
  }
  else {
    ins->op[1].imm = opcode.imm10;
    asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_IMMEDIATE, ins);
  }

  return 4;
}
