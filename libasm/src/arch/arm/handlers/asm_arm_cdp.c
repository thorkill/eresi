/**
** @file asm_arm_cdp.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_cdp(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  LIBASM_PROFILE_FIN();

  ins->instr = ASM_ARM_CDP;

  ins->name = ins->proc->instr_table[ins->instr];

  LIBASM_PROFILE_FOUT(4);
}
