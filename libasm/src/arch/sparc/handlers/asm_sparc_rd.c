/*
**
** $Id: asm_sparc_rd.c,v 1.9 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_rd(asm_instr * ins, unsigned char * buf, unsigned int len,
	     asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_ASSIGN;

  if (opcode.rs1 != 15) { /* RD*(-PR) */
    ins->nb_op = 2;

    ins->op[0].baser = opcode.rd;
    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_REGISTER, ins);
    ins->op[1].baser = opcode.rs1;
    asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_SREGISTER, ins);

    if (ins->op[0].baser == ASM_SREG_Y) {
      ins->instr = ASM_SP_MOV;
    }
  }
  else {
    if (opcode.rd != 0)
      ins->instr = ASM_SP_BAD;
    else if (opcode.i == 0) {	/* STBAR */
      ins->type = ASM_TYPE_OTHER;
      ins->instr = ASM_SP_STBAR;
    }
    else if (opcode.i == 1) {	/* MEMBAR */
      ins->instr = ASM_SP_MEMBAR;
      ins->type = ASM_TYPE_OTHER;
      ins->nb_op = 1;

      /* operand = cmask OR mmask */
      ins->op[0].imm = ((opcode.imm & 0x70) >> 4) | (opcode.imm & 0xf);
      asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_IMMEDIATE, ins);
    }
  }
  
  return 4;
}
