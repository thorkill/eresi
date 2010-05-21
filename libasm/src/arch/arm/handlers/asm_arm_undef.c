#include "libasm.h"

int     asm_arm_undef(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{
  LIBASM_PROFILE_FIN();

  ins->instr = ASM_ARM_;
  MASSIGNTYPE(ins, ASM_TYPE_INT);

  LIBASM_PROFILE_FOUT(4);
}
