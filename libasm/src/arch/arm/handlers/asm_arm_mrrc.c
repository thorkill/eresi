/**
* @file libasm/src/arch/arm/handlers/asm_arm_mrrc.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_mrrc(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{
  struct s_arm_decode_coproc_mov2 opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_coproc_mov2(&opcode, buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = inter->coproc_mov2_table[(opcode.toarm << 4 | opcode.cond)];

  ins->name = ins->proc->instr_table[ins->instr];

  MASSIGNTYPE(ins, ASM_TYPE_ASSIGN);

  ins->nb_op = 5;

  /* Decode operands */
  ins->op[0].imm = opcode.cpnum;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_COPROC, ins);

  ins->op[1].imm = opcode.op;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_IMMEDIATE, ins);

  ins->op[2].baser = opcode.rd;
  ins->op[2].destination = 1;
  asm_arm_op_fetch(&ins->op[2], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[3].baser = opcode.rn;
  ins->op[3].destination = 1;
  asm_arm_op_fetch(&ins->op[3], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[4].baser = opcode.crm;
  asm_arm_op_fetch(&ins->op[4], buf, ASM_ARM_OTYPE_COPROC_REGISTER, ins);

  LIBASM_PROFILE_FOUT(4);
}
