/**
* @file libasm/src/arch/sparc/handlers/asm_sparc_fmovsr.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_fmovsr(asm_instr * ins, u_char * buf, u_int len,
		 asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->type = ASM_TYPE_ASSIGN | ASM_TYPE_COMPARISON;

  ins->instr = inter->fmovr_table[(((opcode.opf & 0x1f) - 6) * 8)
				  + opcode.rcond];

  ins->nb_op = 3;
  ins->op[0].baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op[1].baser = opcode.rs2;
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op[2].baser = opcode.rs1;
  asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_REGISTER, ins);

  return 4;
}
