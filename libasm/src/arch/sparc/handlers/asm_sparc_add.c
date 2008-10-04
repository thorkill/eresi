/*
**
** $Id: asm_sparc_add.c,v 1.8 2007/10/14 00:01:41 heroine Exp $
**
*/
#include "libasm.h"

int 
asm_sparc_add(asm_instr * ins, unsigned char * buf, unsigned int len, 
					asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_ARITH;

  ins->nb_op = 3;  
  ins->op[0].baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_REGISTER, ins);  
  ins->op[2].baser = opcode.rs1;
  asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_REGISTER, ins);

  if (opcode.i == 0) {    
    ins->op[1].baser = opcode.rs2;
    asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_REGISTER, ins);
  }
  else {    
    ins->op[1].imm = opcode.imm;
    asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_IMMEDIATE, ins);
  }

  if ((ins->op[0].baser == ins->op[2].baser) &&
		ins->op[1].content == ASM_SP_OTYPE_IMMEDIATE) {

    ins->instr = ASM_SP_INC;

    if (ins->op[1].imm == 1)
	  ins->nb_op = 1;
    else
	  ins->nb_op = 2;
  }
  
  return 4;
}
