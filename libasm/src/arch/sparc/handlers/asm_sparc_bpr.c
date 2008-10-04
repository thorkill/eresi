/*
**
** $Id: asm_sparc_bpr.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_bpr(asm_instr * ins, unsigned char * buf, unsigned int len,
	      asm_processor * proc)
{
  struct s_decode_rbranch opcoder;
  struct s_asm_proc_sparc *inter;
  sparc_convert_rbranch(&opcoder, buf);

  inter = proc->internals;

  ins->instr = inter->brcc_table[opcoder.rcond];
  ins->type = ASM_TYPE_CONDBRANCH;
  ins->nb_op = 2;
  ins->op[0].imm = opcoder.d16;
  ins->op[1].baser = opcoder.rs1;
  ins->annul = opcoder.a;
  ins->prediction = opcoder.p;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_DISPLACEMENT, ins);
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_REGISTER, ins);

  return 4;
}
