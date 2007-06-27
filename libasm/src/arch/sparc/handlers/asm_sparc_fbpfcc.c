/*
**
** $Id: asm_sparc_fbpfcc.c,v 1.5 2007-06-27 11:25:12 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_fbpfcc(asm_instr * ins, u_char * buf, u_int len,
		 asm_processor * proc)
{
  struct s_decode_pbranch opcodep;
  struct s_asm_proc_sparc *inter;
  sparc_convert_pbranch(&opcodep, buf);

  inter = proc->internals;

  ins->instr = inter->fbcc_table[opcodep.cond];

  if (ins->instr == ASM_SP_FBA)
    ins->type = ASM_TYPE_IMPBRANCH;
  else if (ins->instr == ASM_SP_FBN)
    ins->type = ASM_TYPE_NOP;
  else
    ins->type = ASM_TYPE_CONDBRANCH;

  ins->type = ASM_TYPE_CONDBRANCH;
  ins->nb_op = 2;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_DISPLACEMENT, ins);
  ins->op1.imm = opcodep.imm;
  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_CC, ins);
  ins->op2.baser = opcodep.cc;
  ins->annul = opcodep.a;
  ins->prediction = opcodep.p;
  
  return 4;
}
