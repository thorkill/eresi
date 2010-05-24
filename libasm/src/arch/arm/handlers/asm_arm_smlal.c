/**
* @file libasm/src/arch/arm/handlers/asm_arm_smlal.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_smlal(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  struct s_arm_decode_multiply opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_multiply(&opcode, buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = inter->multiply_table[(opcode.op << 5) | (opcode.cond << 1) | opcode.s];

  ins->name = ins->proc->instr_table[ins->instr];

  MASSIGNTYPE(ins, ASM_TYPE_ARITH);
  if (opcode.s)
    {
      MASSIGNTYPE(ins, ASM_TYPE_WRITEFLAG);
      ins->flagswritten = ASM_ARM_FLAG_N | ASM_ARM_FLAG_Z;
    }

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
