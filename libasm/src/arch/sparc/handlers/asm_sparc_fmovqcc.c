/*
**
** $Id: asm_sparc_fmovqcc.c,v 1.10 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_fmovqcc(asm_instr * ins, unsigned char * buf, unsigned int len,
		  asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->type = ASM_TYPE_ASSIGN | ASM_TYPE_READFLAG;

  if (opcode.opf_cc < 4) {
    ins->instr = inter->fmovfcc_table[(((opcode.opf & 0x1f) - 1) * 8)
				      + opcode.cond];

    ins->flagsread = ASM_SP_FLAG_FCC0 << opcode.opf_cc;
  }
  else if (opcode.opf_cc == 4 || opcode.opf_cc == 6) {
    ins->instr = inter->fmovcc_table[(((opcode.opf & 0x1f) - 1) * 8)
				     + opcode.cond];

    ins->flagsread = ASM_SP_FLAG_C | ASM_SP_FLAG_V | ASM_SP_FLAG_N | ASM_SP_FLAG_Z;
  }
  else {
    ins->instr = ASM_SP_BAD;
    return 4;
  }

  ins->nb_op = 3;
  ins->op[0].baser = ((opcode.rd & 1) << 5) | (opcode.rd & 0x1E);
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op[1].baser = ((opcode.rs2 & 1) << 5) | (opcode.rs2 & 0x1E);
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op[2].baser = opcode.opf_cc;
  asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_CC, ins);

  return 4;
}
