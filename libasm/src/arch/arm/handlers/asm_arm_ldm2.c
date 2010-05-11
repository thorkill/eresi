/**
* @file libasm/src/arch/arm/handlers/asm_arm_ldm.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_ldm2(asm_instr * ins, u_char * buf, u_int len,
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

  ins->type = ASM_TYPE_LOAD;

  ins->nb_op = 2;

  /* Decode operands */
  ins->op[0].baser = opcode.rn;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[1].imm = opcode.reg_list;
  ins->op[1].destination = 1;
  /* User mode registers are loaded */
  ins->op[1].regset = ASM_ARM_REGSET_USR;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_REG_LIST, ins);

  LIBASM_PROFILE_FOUT(4);
}
