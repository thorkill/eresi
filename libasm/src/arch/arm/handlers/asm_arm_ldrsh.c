#include "libasm.h"

int     asm_arm_ldrsh(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  struct s_arm_decode_ldst_misc opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_ldst_misc(&opcode,buf);

  ins->instr = inter->ldst_table[(opcode.l << 6) | (opcode.s << 5) | opcode.h << 4 | opcode.cond];

  ins->name = ins->proc->instr_table[ins->instr];

  ins->type = ASM_TYPE_LOAD;

  ins->nb_op = 2;

  /* Decode operands */
  ins->op[0].baser = opcode.rd;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  arm_decode_ldst_misc_offop(ins, buf, 1, &opcode);

  LIBASM_PROFILE_FOUT(4);
}
