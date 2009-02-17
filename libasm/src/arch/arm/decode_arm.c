#include "libasm.h"

void	arm_decode_dataproc_shfop(asm_instr *ins, u_char *buf, u_int op_nr,
                                  struct s_arm_decode_dataproc *opcode)
{
  asm_operand *op;

  op = &ins->op[op_nr];

  if (opcode->i)
    {
      u_int64_t temp;

      temp = opcode->immed << (32 - opcode->rotate_imm*2);
      temp = temp | (temp >> 32);
      op->imm = (u_int32_t) temp;
      asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_IMMEDIATE, ins);
    }
  else
    {
      op->baser = opcode->rm;
      op->shift_type = opcode->shift;

      if (opcode->reg_shift)
        {
          /* Scaled by register */
          op->indexr = opcode->rs;
          op->imm = 0;
          asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_REG_SCALED, ins);
        }
      else
        {
          /* Scaled by immediate */
          op->imm = opcode->shift_imm;
          op->indexr = ASM_ARM_REG_NUM;

          if (opcode->shift_imm == 0 && opcode->shift == ASM_ARM_SHIFT_LSL)
            asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_REGISTER, ins);
          else
            asm_arm_op_fetch(op, buf, ASM_ARM_OTYPE_REG_SCALED, ins);
        }
    }
}
