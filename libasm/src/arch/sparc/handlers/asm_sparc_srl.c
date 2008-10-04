/*
**
** $Id: asm_sparc_srl.c,v 1.8 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_srl(asm_instr * ins, unsigned char * buf, unsigned int len,
	      asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->type = ASM_TYPE_ARITH;

  ins->instr = inter->shift_table[opcode.op3 - 0x25 +
				  ((opcode.imm & 0x1000) ? 1 : 0)];

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
    ins->op[1].imm = opcode.shcnt;
    asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_IMMEDIATE, ins);
  }

  /* Synthetics */
  if (ins->op[1].content == ASM_SP_OTYPE_REGISTER &&
      ins->op[1].baser == ASM_REG_G0) {

    ins->instr = ASM_SP_CLRUW;
    if (ins->op[2].baser == ins->op[0].baser)
      ins->nb_op = 1;
    else {
      ins->nb_op = 2;
      ins->op[1] = ins->op[2];
    }
  }

  return 4;
}
