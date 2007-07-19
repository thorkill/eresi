/*
**
** $Id: asm_sparc_fcmpes.c,v 1.9 2007-07-19 07:20:55 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_fcmpes(asm_instr * ins, u_char * buf, u_int len,
		 asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  ins->flagswritten = ASM_SP_FLAG_FCC0 << opcode.cc;

  ins->instr = inter->fcmp_table[(opcode.opf & 0x1f) - 16];
  ins->nb_op = 3;
  ins->op1.baser = opcode.rs2;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op2.baser = opcode.rs1;
  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op3.baser = opcode.cc;
  asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_CC, ins);
  
  return 4;
}
