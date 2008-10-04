/*
**
** $Id: asm_sparc_call.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include "libasm.h"

int 
asm_sparc_call(asm_instr *ins, unsigned char *buf, unsigned int len, 
       			asm_processor *proc) 
{		       	
  struct s_decode_call	opcode;  
  sparc_convert_call(&opcode, buf);
  
  ins->ptr_instr = buf;  
  ins->instr = ASM_SP_CALL;
  ins->type = ASM_TYPE_CALLPROC;
  ins->nb_op = 1;
  ins->op[0].imm = opcode.displacement;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_DISP30, ins);

  return 4;
}
