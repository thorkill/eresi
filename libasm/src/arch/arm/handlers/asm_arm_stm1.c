/**
* @file libasm/src/arch/arm/handlers/asm_arm_stm.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_stm1(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  struct s_arm_decode_ldst_mult opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_ldst_mult(&opcode, buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = inter->ldst_mult_table[(opcode.l << 6) | (opcode.cond << 2) | (opcode.p << 1) | opcode.u];

  ins->name = ins->proc->instr_table[ins->instr];

  MASSIGNTYPE(ins, ASM_TYPE_STORE);

  ins->nb_op = 2;

  /* Decode operands */
  ins->op[0].baser = opcode.rn;
  if (opcode.w)
    ins->op[0].indexing = ASM_ARM_ADDRESSING_POSTINDEXED;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[1].imm = opcode.reg_list;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_REG_LIST, ins);

  LIBASM_PROFILE_FOUT(4);
}
