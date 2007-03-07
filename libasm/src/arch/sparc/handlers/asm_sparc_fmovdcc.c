/*
**
** $Id: asm_sparc_fmovdcc.c,v 1.3 2007-03-07 16:45:34 thor Exp $
**
*/
#include "libasm.h"

int
asm_sparc_fmovdcc(asm_instr * ins, u_char * buf, u_int len,
		  asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_STORE;

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
  ins->op1.type = ASM_SP_OTYPE_FREGISTER;
  ins->op1.base_reg = opcode.rd;
  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
  ins->op2.base_reg = opcode.rs2;
  ins->op3.type = ASM_SP_OTYPE_CC;
  ins->op3.base_reg = opcode.opf_cc;
  	  	  
  return 4;
}
