/*
**
** $Id: asm_sparc_illegal.c,v 1.3 2007/08/07 07:13:27 may Exp $
**
*/
#include "libasm.h"

int
asm_sparc_illegal(asm_instr * ins, unsigned char * buf, unsigned int len,
		  asm_processor * proc)
{
  ins->instr = ASM_SP_BAD;
  ins->type  = ASM_TYPE_STOP;
  return 4;
}
