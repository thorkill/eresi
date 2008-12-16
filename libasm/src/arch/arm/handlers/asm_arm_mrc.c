#include "libasm.h"

int     asm_arm_mrc(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  LIBASM_PROFILE_FIN();

  ins->instr = ASM_ARM_MRC;

  LIBASM_PROFILE_FOUT(4);
}
