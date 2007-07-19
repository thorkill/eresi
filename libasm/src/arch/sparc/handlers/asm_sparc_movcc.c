/*
**
** $Id: asm_sparc_movcc.c,v 1.9 2007-07-19 07:20:55 strauss Exp $
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
  ins->type = ASM_TYPE_ASSIGN | ASM_TYPE_READFLAG;

  if (opcode4.cc2 == 0) { /* fcc{0,1,2,3,4} */
   	ins->instr = inter->movcc_table[opcode4.cond];  	  	
    ins->flagsread = ASM_SP_FLAG_FCC0 << opcode4.cc;
  }
  else if (opcode4.cc0 == 0) { /* icc or xcc */
   	ins->instr = inter->movfcc_table[opcode4.cond];
    ins->flagsread = ASM_SP_FLAG_C | ASM_SP_FLAG_V | ASM_SP_FLAG_N | ASM_SP_FLAG_Z;
  }
  else {
    ins->instr = ASM_SP_BAD;
    return 4;
  }

  ins->nb_op = 3;
  ins->op1.baser = opcode4.rd;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op3.baser = opcode4.cc;
  asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_CC, ins);

  if (opcode.i == 0) {
    ins->op2.baser = opcode.rs2;
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_REGISTER, ins);
  }
  else {
    ins->op2.imm = opcode.imm;
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_IMMEDIATE, ins);
  }

  return 4;
}
