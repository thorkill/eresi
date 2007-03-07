/*
**
** $Id: asm_sparc_rdpr.c,v 1.3 2007-03-07 16:45:35 thor Exp $
**
*/
#include "libasm.h"

int
asm_sparc_rdpr(asm_instr * ins, u_char * buf, u_int len,
	       asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_LOAD;

  if (opcode.rs1 < ASM_PREG_BAD16 || opcode.rs1 > ASM_PREG_BAD30) {
    ins->nb_op = 2;
    ins->op1.type = ASM_SP_OTYPE_REGISTER;
    ins->op1.base_reg = opcode.rd;
    ins->op2.type = ASM_SP_OTYPE_PREGISTER;
    ins->op2.base_reg = opcode.rs1;
  }
  else
    ins->instr = ASM_SP_BAD;
    
  return 4;

}
