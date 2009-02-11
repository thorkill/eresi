#include "libasm.h"

int     asm_arm_ldr(asm_instr * ins, u_char * buf, u_int len,
                   asm_processor * proc)
{
  struct s_arm_decode_ldst opcode;

  LIBASM_PROFILE_FIN();

  arm_convert_ldst(&opcode,buf);

  ins->instr = ASM_ARM_LDR;

  ins->name = ins->proc->instr_table[ins->instr];

  ins->type = ASM_TYPE_LOAD;

  ins->nb_op = 3;

  /* Decode operands */
  ins->op[0].baser = opcode.rd;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);
  ins->op[1].baser = opcode.rn;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_REGISTER, ins);

  // TODO: continue from HERE!!

  if (!opcode.reg_offset)
    {

    }

  LIBASM_PROFILE_FOUT(4);
}
