/**
* @file libasm/src/arch/arm/handlers/asm_arm_mrc.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_mrc(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  LIBASM_PROFILE_FIN();

  ins->instr = ASM_ARM_MRC;

  ins->name = ins->proc->instr_table[ins->instr];

  LIBASM_PROFILE_FOUT(4);
}
