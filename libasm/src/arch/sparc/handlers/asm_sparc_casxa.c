/*
**
** $Id: asm_sparc_casxa.c,v 1.10 2007/10/14 00:01:41 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_casxa(asm_instr * ins, unsigned char * buf, unsigned int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  ins->type = ASM_TYPE_LOAD | ASM_TYPE_ASSIGN | ASM_TYPE_COMPARISON | ASM_TYPE_STORE;

  ins->nb_op = 3;
  ins->op[0].baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op[1].baser = opcode.rs2;
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_REGISTER, ins);

  if (opcode.i) {
    ins->op[2].baser = opcode.rs1;
    ins->op[2].imm = 0;
    ins->op[2].address_space = -1;
    asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_IMM_ADDRESS, ins);
  }
  else {
    ins->op[2].baser = opcode.rs1;
    ins->op[2].indexr = -1;
    ins->op[2].address_space = opcode.none;

    /* Synthethics */
    if (ins->op[2].address_space == ASM_SP_ASI_P)
      ins->instr = ASM_SP_CASX;
    else if (ins->op[2].address_space == ASM_SP_ASI_P_L) {
      ins->instr = ASM_SP_CASXL;
      ins->op[2].address_space = ASM_SP_ASI_P;
    }

    asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_REG_ADDRESS, ins);
  }

  return 4;

}
