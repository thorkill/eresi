/*
**
** $Id: asm_sparc_sethi.c,v 1.8 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_sethi(asm_instr * ins, unsigned char * buf, unsigned int len,
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
    ins->op[0].baser = opcode.rd;
    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_REGISTER, ins);
    ins->op[1].imm = opcode.imm;
    asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_SETHI, ins);
  }

  return 4;
}
