/*
**
** $Id: asm_sparc_rdpr.c,v 1.8 2007/10/14 00:01:42 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_rdpr(asm_instr * ins, unsigned char * buf, unsigned int len,
	       asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_ASSIGN;

  if (opcode.rs1 < ASM_PREG_BAD16 || opcode.rs1 > ASM_PREG_BAD30) {
    ins->nb_op = 2;
    ins->op[0].baser = opcode.rd;
    asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_REGISTER, ins);
    ins->op[1].baser = opcode.rs1;
    asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_PREGISTER, ins);
  }
  else
    ins->instr = ASM_SP_BAD;

  return 4;
}
