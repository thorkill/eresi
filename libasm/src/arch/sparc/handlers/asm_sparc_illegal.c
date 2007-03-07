/*
**
** $Id: asm_sparc_illegal.c,v 1.2 2007-03-07 16:45:34 thor Exp $
**
*/
#include "libasm.h"

int
asm_sparc_illegal(asm_instr * ins, u_char * buf, u_int len,
		  asm_processor * proc)
{
  ins->instr = ASM_SP_BAD;
  return 4;
}
