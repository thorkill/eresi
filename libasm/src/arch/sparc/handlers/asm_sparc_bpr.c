/*
**
** $Id: asm_sparc_bpr.c,v 1.4 2007-06-27 11:25:12 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_bpr(asm_instr * ins, u_char * buf, u_int len,
	      asm_processor * proc)
{
  struct s_decode_rbranch opcoder;
  struct s_asm_proc_sparc *inter;
  sparc_convert_rbranch(&opcoder, buf);

  inter = proc->internals;

  ins->instr = inter->brcc_table[opcoder.rcond];
  ins->type = ASM_TYPE_CONDBRANCH;
  ins->nb_op = 2;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_DISPLACEMENT, ins);
  ins->op1.imm = opcoder.d16;
  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op2.baser = opcoder.rs1;
  ins->annul = opcoder.a;
  ins->prediction = opcoder.p;

  return 4;
}
