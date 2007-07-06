/*
**
** $Id: asm_sparc_sethi.c,v 1.7 2007-07-06 21:18:08 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_sethi(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_branch opcode;
  sparc_convert_branch(&opcode, buf);
  
  ins->type = ASM_TYPE_ASSIGN;

  if (!opcode.rd && !opcode.imm) {
    ins->instr = ASM_SP_NOP;
    ins->type = ASM_TYPE_NOP;
    ins->nb_op = 0;
  }
  else {
    ins->instr = ASM_SP_SETHI;
    ins->nb_op = 2;
    ins->op1.baser = opcode.rd;
    asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_REGISTER, ins);
    ins->op2.imm = opcode.imm;
    asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_SETHI, ins);
  }

  return 4;
}
