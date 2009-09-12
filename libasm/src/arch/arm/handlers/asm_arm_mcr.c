/**
** @file asm_arm_mcr.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_mcr(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  LIBASM_PROFILE_FIN();

  ins->instr = ASM_ARM_MCR;

  ins->name = ins->proc->instr_table[ins->instr];

  LIBASM_PROFILE_FOUT(4);
}
