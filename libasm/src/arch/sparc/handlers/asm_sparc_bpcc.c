/*
**
** $Id: asm_sparc_bpcc.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_bpcc(asm_instr * ins, unsigned char * buf, unsigned int len,
	       asm_processor * proc)
{
  struct s_decode_pbranch opcodep;
  struct s_asm_proc_sparc *inter;
  sparc_convert_pbranch(&opcodep, buf);

  inter = proc->internals;

  ins->instr = inter->bcc_table[opcodep.cond];

  if (ins->instr == ASM_SP_BA)
    ins->type = ASM_TYPE_IMPBRANCH;
  else if (ins->instr == ASM_SP_BN)
    ins->type = ASM_TYPE_NOP;
  else
    ins->type = ASM_TYPE_CONDBRANCH;

  ins->nb_op = 2;  
  ins->op[0].imm = opcodep.imm;  
  ins->op[1].baser = opcodep.cc + 4;
  ins->annul = opcodep.a;
  ins->prediction = opcodep.p;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_DISPLACEMENT, ins);
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_CC, ins);

  if (ins->instr == ASM_SP_BN &&
      ins->annul && ins->prediction && ins->op[1].baser == ASM_SP_XCC) {

    ins->instr = ASM_SP_IPREFETCH;
    ins->nb_op = 1;
    ins->type = ASM_TYPE_NONE;
  }

  return 4;
}
