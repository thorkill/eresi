#include "libasm.h"

int     asm_arm_msr(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  LIBASM_PROFILE_FIN();

  ins->instr = ASM_ARM_MSR;

  LIBASM_PROFILE_FOUT(4);
}
