/**
* @file libasm/src/arch/arm/handlers/asm_arm_mov.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_mov(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{
  struct s_arm_decode_dataproc opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_dataproc(&opcode, buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = inter->dataproc_table[(opcode.op2 << 5) | (opcode.cond << 1) | opcode.s];

  ins->name = ins->proc->instr_table[ins->instr];

  ins->type = ASM_TYPE_ASSIGN;

  ins->nb_op = 2;

  /* Decode operands */
  ins->op[0].baser = opcode.rd;
  ins->op[0].destination = 1;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  arm_decode_dataproc_shfop(ins, buf, 1, &opcode);

  /* Decode flags related behaviour */
  arm_decode_dataproc_flagswritten(ins, &opcode);

  /* Check if this MOV is a procedure return (MOV PC,LR) */
  if ((ins->op[1].content & ASM_ARM_OTYPE_REGISTER) == ASM_ARM_OTYPE_REGISTER
      && ins->op[1].baser == ASM_ARM_REG_R14
      && ins->op[0].baser == ASM_ARM_REG_PC)
    {
      /* clear types assigned in the operand handler */
      ins->type &= ~(ASM_TYPE_BRANCH);
      /* assign the real type */
      ins->type |= ASM_TYPE_RETPROC;
    }

  LIBASM_PROFILE_FOUT(4);
}
