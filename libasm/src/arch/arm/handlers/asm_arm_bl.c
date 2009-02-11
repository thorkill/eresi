#include "libasm.h"

int     asm_arm_bl(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  LIBASM_PROFILE_FIN();

  ins->instr = ASM_ARM_BL;

  ins->name = ins->proc->instr_table[ins->instr];

  ins->nb_op = 0;

  LIBASM_PROFILE_FOUT(4);
}
