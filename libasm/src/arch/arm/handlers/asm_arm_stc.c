/**
** @file asm_arm_stc.c
** @ingroup a-instrs
*/
#include "libasm.h"

int     asm_arm_stc(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  LIBASM_PROFILE_FIN();

  ins->instr = ASM_ARM_STC;

  ins->name = ins->proc->instr_table[ins->instr];

  LIBASM_PROFILE_FOUT(4);
}
