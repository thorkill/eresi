#include "libasm.h"

int     asm_arm_cmn(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{
  LIBASM_PROFILE_FIN();

  ins->instr = ASM_ARM_CMN;

  LIBASM_PROFILE_FOUT(4);
}
