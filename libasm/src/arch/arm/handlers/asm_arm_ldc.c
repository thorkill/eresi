/**
* @file libasm/src/arch/arm/handlers/asm_arm_ldc.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_ldc(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  struct s_arm_decode_coproc_ldst opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_coproc_ldst(&opcode, buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = inter->coproc_ldst_table[(opcode.l << 5) | (opcode.cond << 1) | opcode.n];

  ins->name = ins->proc->instr_table[ins->instr];

  MASSIGNTYPE(ins, ASM_TYPE_LOAD);

  ins->nb_op = 3;

  /* Decode operands */
  ins->op[0].imm = opcode.cpnum;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_COPROC, ins);

  ins->op[1].baser = opcode.crd;
  ins->op[1].destination = 1;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_COPROC_REGISTER, ins);

  arm_decode_coproc_ldst_offop(ins, buf, 2, &opcode);

  LIBASM_PROFILE_FOUT(4);
}
