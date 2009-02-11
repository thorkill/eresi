#include "libasm.h"

int     asm_arm_orr(asm_instr * ins, u_char * buf, u_int len,
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

  if (opcode.i)
    {
      u_int64_t temp;

      temp = opcode.immed << (32 - opcode.rotate_imm*2);
      temp = temp | (temp >> 32);
      ins->op[2].imm = (u_int32_t) temp;
      asm_arm_op_fetch(&ins->op[2], buf, ASM_ARM_OTYPE_IMMEDIATE, ins);
    }
  else
    {
      ins->op[2].baser = opcode.rm;
      ins->op[2].shift_type = opcode.shift;

      if (opcode.reg_shift)
        {
          /* Shifted by register */
          ins->op[2].indexr = opcode.rs;
          asm_arm_op_fetch(&ins->op[2], buf, ASM_ARM_OTYPE_REG_SHIFTED_REG, ins);
        }
      else
        {
          /* Shifted by immediate */
          ins->op[2].imm = opcode.shift_imm;

          if (opcode.shift_imm == 0 && opcode.shift == ASM_ARM_SHIFT_LSL)
            asm_arm_op_fetch(&ins->op[2], buf, ASM_ARM_OTYPE_REGISTER, ins);
          else
            asm_arm_op_fetch(&ins->op[2], buf, ASM_ARM_OTYPE_REG_SHIFTED_IMM, ins);
        }
    }

  LIBASM_PROFILE_FOUT(4);
}
