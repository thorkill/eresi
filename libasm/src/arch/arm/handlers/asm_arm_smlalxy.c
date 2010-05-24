/**
* @file libasm/src/arch/arm/handlers/asm_arm_smlalxy.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_smlalxy(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{

  struct s_arm_decode_multiply opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_multiply(&opcode, buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = inter->dsp_multiply_table[(opcode.op << 6) | (opcode.cond << 2) | (opcode.y << 1) | opcode.x];

  ins->name = ins->proc->instr_table[ins->instr];

  MASSIGNTYPE(ins, ASM_TYPE_ARITH);

  ins->nb_op = 4;

  /* Decode operands */
  arm_decode_multiply_long(ins, buf, &opcode);

  if (MISTYPE(ins, ASM_TYPE_BRANCH)
      || MISTYPE(ins, ASM_TYPE_CALLPROC)
      || MISTYPE(ins, ASM_TYPE_RETPROC))
    {
      MASSIGNTYPE(ins, ASM_TYPE_INDCONTROL);
    }

  LIBASM_PROFILE_FOUT(4);

}
