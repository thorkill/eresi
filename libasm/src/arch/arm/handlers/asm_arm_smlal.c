#include "libasm.h"

int     asm_arm_smlal(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  LIBASM_PROFILE_FIN();

  ins->instr = ASM_ARM_SMLAL;

  LIBASM_PROFILE_FOUT(4);
}
