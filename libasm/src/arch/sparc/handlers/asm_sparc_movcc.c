/*
**
** $Id: asm_sparc_movcc.c,v 1.4 2007-04-22 20:48:41 strauss Exp $
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
  sparc_convert_format3(&opcode, buf, proc);
  sparc_convert_format4(&opcode4, buf, proc);

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
  ins->op1.type = ASM_SP_OTYPE_REGISTER;
  ins->op1.base_reg = opcode4.rd;
  ins->op3.type = ASM_SP_OTYPE_CC;
  ins->op3.base_reg = opcode4.cc;
  	  	
  if (opcode.i == 0) {
  	ins->op2.type = ASM_SP_OTYPE_REGISTER;
  	ins->op2.base_reg = opcode.rs2;
  }
  else {  		
  	ins->op2.type = ASM_SP_OTYPE_IMMEDIATE;
   	ins->op2.imm = opcode.imm;
  }
  
  return 4;
  
}
