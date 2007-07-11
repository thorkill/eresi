/*
**
** $Id: asm_sparc_fmovqcc.c,v 1.8 2007-07-11 22:06:47 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_fmovqcc(asm_instr * ins, u_char * buf, u_int len,
		  asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->type = ASM_TYPE_ASSIGN;

  if (opcode.opf_cc < 4)
    ins->instr = inter->fmovfcc_table[(((opcode.opf & 0x1f) - 1) * 8)
				      + opcode.cond];
  else if (opcode.opf_cc == 4 || opcode.opf_cc == 6)
    ins->instr = inter->fmovcc_table[(((opcode.opf & 0x1f) - 1) * 8)
				     + opcode.cond];
  else {
    ins->instr = ASM_SP_BAD;
    return 4;
  }

  ins->nb_op = 3;
  ins->op1.baser = ((opcode.rd & 1) << 5) | (opcode.rd & 0x1E);
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op2.baser = ((opcode.rs2 & 1) << 5) | (opcode.rs2 & 0x1E);
  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op3.baser = opcode.opf_cc;
  asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_CC, ins);

  return 4;
}
