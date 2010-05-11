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

  ins->instr = inter->dsp_multiply_table[(opcode.op << 4) | opcode.cond];

  ins->name = ins->proc->instr_table[ins->instr];

  ins->type = ASM_TYPE_ARITH;

  ins->nb_op = 4;

  /* Decode operands */
  // TODO: verify if we can do like smlal
  ins->op[0].baser = opcode.r2; /* This is RdLo */
  ins->op[0].destination = 1;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[1].baser = opcode.r1; /* This is RdHi */
  ins->op[1].destination = 1;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[2].baser = opcode.r4; /* This is Rm */
  asm_arm_op_fetch(&ins->op[2], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[3].baser = opcode.r3; /* This is Rs */
  asm_arm_op_fetch(&ins->op[3], buf, ASM_ARM_OTYPE_REGISTER, ins);

  LIBASM_PROFILE_FOUT(4);

}
