/**
* @file libasm/src/arch/arm/handlers/asm_arm_pld.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_pld(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{
  struct s_arm_decode_ldst opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_ldst(&opcode,buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = ASM_ARM_PLD;

  ins->name = ins->proc->instr_table[ins->instr];

  MASSIGNTYPE(ins, ASM_TYPE_NOP | ASM_TYPE_ARCH);

  ins->nb_op = 1;

  /* Decode operands */
  arm_decode_ldst_offop(ins, buf, 0, &opcode);

  LIBASM_PROFILE_FOUT(4);
}
