/*
**
** $Id: asm_sparc_call.c,v 1.3 2007-06-16 20:24:25 strauss Exp $
**
*/
#include "libasm.h"

int 
asm_sparc_call(asm_instr *ins, u_char *buf, u_int len, 
       			asm_processor *proc) 
{		       	
  struct s_decode_call	opcode;  
  sparc_convert_call(&opcode, buf);
  
  ins->ptr_instr = buf;  
  ins->instr = ASM_SP_CALL;
  ins->type = ASM_TYPE_CALLPROC;
  ins->nb_op = 1;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_DISP30, ins);
  ins->op1.imm = opcode.displacement;  
  
  return 4;
}
