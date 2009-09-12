/**
** @file asm_arm_add.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_add(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{
  struct s_arm_decode_dataproc opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_dataproc(&opcode, buf);

  ins->instr = inter->dataproc_table[(opcode.op2 << 5) | (opcode.cond << 1) | opcode.s];

  ins->name = ins->proc->instr_table[ins->instr];

  ins->type = ASM_TYPE_ARITH;

  ins->nb_op = 3;

  /* Decode operands */
  ins->op[0].baser = opcode.rd;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);
  ins->op[1].baser = opcode.rn;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_REGISTER, ins);

  arm_decode_dataproc_shfop(ins, buf, 2, &opcode);

  LIBASM_PROFILE_FOUT(4);

}
