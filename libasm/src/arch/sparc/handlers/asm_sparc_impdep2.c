/*
**
** $Id: asm_sparc_impdep2.c,v 1.4 2007/06/16 20:24:25 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_impdep2(asm_instr * ins, unsigned char * buf, unsigned int len,
		  asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);
  
  ins->type = ASM_TYPE_ARCH;

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];

  return 4;
}
