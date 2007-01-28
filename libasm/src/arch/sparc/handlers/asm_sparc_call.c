#include "libasm.h"

int 
asm_sparc_call(asm_instr *ins, u_char *buf, u_int len, 
       			asm_processor *proc) 
{		       	
  struct s_decode_call	opcode;  
  sparc_convert_call(&opcode, buf, proc);
  
  ins->ptr_instr = buf;  
  ins->instr = ASM_SP_CALL;
  ins->type = ASM_TYPE_CALLPROC;
  ins->nb_op = 1;
  ins->op1.type = ASM_SP_OTYPE_DISP30;
  ins->op1.imm = opcode.displacement;  
  
  return 4;
}
