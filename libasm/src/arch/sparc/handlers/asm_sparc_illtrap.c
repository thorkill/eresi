/*
**
** $Id: asm_sparc_illtrap.c,v 1.8 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_illtrap(asm_instr * ins, unsigned char * buf, unsigned int len,
		  asm_processor * proc)
{
  struct s_decode_branch opcode;
  sparc_convert_branch(&opcode, buf);
  
  ins->type = ASM_TYPE_INT | ASM_TYPE_STOP;

  ins->instr = ASM_SP_ILLTRAP;
  ins->nb_op = 1;
  ins->op[0].imm = opcode.immediate;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_IMMEDIATE, ins);

  return 4;
}
