/*
**
** $Id: asm_sparc_movcc.c,v 1.5 2007-06-16 20:24:25 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_movcc(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_decode_format4 opcode4;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  sparc_convert_format4(&opcode4, buf);

  inter = proc->internals;
  ins->type = ASM_TYPE_ASSIGN;

  if (opcode4.cc2 == 0) { /* fcc{0,1,2,3,4} */
   	ins->instr = inter->movcc_table[opcode4.cond];  	  	
  }
  else if (opcode4.cc0 == 0) { /* icc or xcc */
   	ins->instr = inter->movfcc_table[opcode4.cond];
  }
  else {
    ins->instr = ASM_SP_BAD;
    return 4;
  }
  	  
  ins->nb_op = 3;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op1.base_reg = opcode4.rd;
  asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_CC, ins);
  ins->op3.base_reg = opcode4.cc;
  	  	
  if (opcode.i == 0) {
  	asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
  	ins->op2.base_reg = opcode.rs2;
  }
  else {  		
  	asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMMEDIATE, ins);
   	ins->op2.imm = opcode.imm;
  }
  
  return 4;
  
}
