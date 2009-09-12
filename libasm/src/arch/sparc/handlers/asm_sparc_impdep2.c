/**
** @file asm_sparc_impdep2.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_impdep2(asm_instr * ins, u_char * buf, u_int len,
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
