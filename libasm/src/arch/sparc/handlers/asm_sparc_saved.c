/*
**
** $Id: asm_sparc_saved.c,v 1.6 2007/08/07 07:13:27 may Exp $
**
*/
#include "libasm.h"

int
asm_sparc_saved(asm_instr * ins, unsigned char * buf, unsigned int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_PROLOG;

  if (opcode.rd == 0)
    ins->instr = ASM_SP_SAVED;
  else if (opcode.rd == 1)
    ins->instr = ASM_SP_RESTORED;
  else
    ins->instr = ASM_SP_BAD;

  return 4;
}
