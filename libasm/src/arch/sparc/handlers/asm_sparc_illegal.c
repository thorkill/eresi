/**
* @file libasm/src/arch/sparc/handlers/asm_sparc_illegal.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_illegal(asm_instr * ins, u_char * buf, u_int len,
		  asm_processor * proc)
{
  ins->instr = ASM_SP_BAD;
  ins->type  = ASM_TYPE_STOP;
  return 4;
}
