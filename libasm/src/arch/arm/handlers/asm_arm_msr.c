/**
* @file libasm/src/arch/arm/handlers/asm_arm_msr.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_msr(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  struct s_arm_decode_dataproc opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_dataproc(&opcode, buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = inter->movpsr_table[(opcode.topsr << 4) | opcode.cond];

  ins->name = ins->proc->instr_table[ins->instr];

  MASSIGNTYPE(ins, ASM_TYPE_ASSIGN | ASM_TYPE_ARCH);

  ins->nb_op = 2;

  /* Decode operands */
  ins->op[0].baser = (opcode.r ? ASM_ARM_REG_SPSR : ASM_ARM_REG_CPSR);
  ins->op[0].imm = opcode.field_mask;
  ins->op[0].destination = 1;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  arm_decode_dataproc_shfop(ins, buf, 1, &opcode);

  LIBASM_PROFILE_FOUT(4);
}
