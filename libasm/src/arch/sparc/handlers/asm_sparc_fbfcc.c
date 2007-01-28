#include "libasm.h"

int
asm_sparc_fbfcc(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_branch opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_branch(&opcode, buf, proc);

  inter = proc->internals;

  ins->instr = inter->fbcc_table[opcode.cond];
  ins->type = ASM_TYPE_CONDBRANCH;
  ins->nb_op = 1;
  ins->op1.type = ASM_SP_OTYPE_DISPLACEMENT;
  ins->op1.imm = opcode.imm;
  ins->annul = opcode.a;
  /* This will avoid having ',pn' on non-prediction branches outputs */
  ins->prediction = 1;
  
  return 4;
}
