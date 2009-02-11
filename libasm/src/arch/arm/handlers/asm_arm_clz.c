#include "libasm.h"

int     asm_arm_clz(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{
  struct s_arm_decode_dataproc opcode;

  LIBASM_PROFILE_FIN();

  arm_convert_dataproc(&opcode, buf);

  ins->instr = ASM_ARM_CLZ;

  ins->name = ins->proc->instr_table[ins->instr];

  ins->type = ASM_TYPE_BITTEST;

  ins->nb_op = 2;

  /* Decode operands */
  ins->op[0].baser = opcode.rd;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);
  ins->op[1].baser = opcode.rm;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_REGISTER, ins);

  LIBASM_PROFILE_FOUT(4);
}
