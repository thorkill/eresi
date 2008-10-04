/*
**
** $Id: asm_sparc_tcc.c,v 1.10 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_tcc(asm_instr * ins, unsigned char * buf, unsigned int len,
	      asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_decode_format4 opcode4;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  sparc_convert_format4(&opcode4, buf);

  inter = proc->internals;
  ins->type = ASM_TYPE_INT | ASM_TYPE_READFLAG;
  ins->flagsread = ASM_SP_FLAG_C | ASM_SP_FLAG_V | ASM_SP_FLAG_N | ASM_SP_FLAG_Z;

  ins->instr = inter->tcc_table[opcode4.cond];
  ins->nb_op = 2;
  if (opcode4.i) {
    ins->op[0].baser = opcode4.rs1;
    ins->op[0].imm = opcode4.sw_trap;
    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
  }
  else {
    ins->op[0].baser = opcode4.rs1;
    ins->op[0].indexr = opcode4.rs2;
    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
  }

  ins->op[1].baser = (opcode4.cc & 0x3) + 4;
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_CC, ins);

  return 4;
}
