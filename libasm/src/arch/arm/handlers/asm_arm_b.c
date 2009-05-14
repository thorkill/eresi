#include "libasm.h"

int     asm_arm_b(asm_instr * ins, u_char * buf, u_int len, 
                                        asm_processor * proc)
{
  struct s_arm_decode_branch1 opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_branch1(&opcode, buf);

  ins->instr = inter->branch1_table[(opcode.l_h << 4) | opcode.cond];

  ins->name = ins->proc->instr_table[ins->instr];

  ins->type = (ins->instr == ASM_ARM_B) ? ASM_TYPE_IMPBRANCH : ASM_TYPE_CONDBRANCH;

  ins->nb_op = 1;

  /* Decode operands */
  ins->op[0].imm = opcode.signed_imm;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_DISP, ins);

  LIBASM_PROFILE_FOUT(4);
}
