/*
**
** $Id: asm_sparc_fcmpeq.c,v 1.4 2007-04-22 20:48:41 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_fcmpeq(asm_instr * ins, u_char * buf, u_int len,
		 asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_TEST;

  ins->instr = inter->fcmp_table[(opcode.opf & 0x1f) - 16];
  ins->nb_op = 3;
  ins->op1.type = ASM_SP_OTYPE_FREGISTER;
  ins->op1.base_reg = opcode.rs2;
  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
  ins->op2.base_reg = opcode.rs1;
  ins->op3.type = ASM_SP_OTYPE_CC;
  ins->op3.base_reg = opcode.cc;
  
  return 4;
}
