/*
**
** $Id: asm_sparc_stha.c,v 1.7 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_stha(asm_instr * ins, unsigned char * buf, unsigned int len,
	       asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  ins->type = ASM_TYPE_STORE | ASM_TYPE_ASSIGN;

  ins->nb_op = 2;
  if (opcode.i) {
    ins->op[0].baser = opcode.rs1;
    ins->op[0].imm = opcode.imm;
    ins->op[0].address_space = -1;
    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
  }
  else {
    ins->op[0].baser = opcode.rs1;
    ins->op[0].indexr = opcode.rs2;
    ins->op[0].address_space = opcode.none;
    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
  }

  ins->op[1].baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_REGISTER, ins);

  return 4;
}
